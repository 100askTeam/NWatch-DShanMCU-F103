/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#define NOALARM	UCHAR_MAX

static byte nextAlarm;
static byte nextAlarmDay;
static bool alarmSetOff;
static draw_f oldDrawFunc;
static button_f oldBtn1Func;
static button_f oldBtn2Func;
static button_f oldBtn3Func;

static volatile alarm_s flashAlarms[ALARM_COUNT] EEMEM = {{22, 45, 0},{01, 48, 4},{7, 45, 63},{9, 4, 0},{0, 1, 0}};
 //测试出来占了3个字节


static bool goingOff(void);
static void getNextAlarm(void);
static uint toMinutes(byte, byte, byte);
static bool stopAlarm(void);
static display_t draw(void);


#define  APPCONFIGSIZE  8

void alarm_init()
{
	uint8_t try_count = 3;
	uint32_t flash_test_data = 0;
	alarm_s flashAlarms_test_datas = {0, 0, 0};
	
	W25Q64_Read(FLASH_SECTOR_ALARM, (uint8_t *)&flash_test_data, 4);
	while(try_count--)
	{
		if((flash_test_data != 0) || (flash_test_data != 0xff))
		{
			HAL_Delay(1);
			W25Q64_Read(FLASH_SECTOR_ALARM, (uint8_t *)&flashAlarms_test_datas, sizeof(alarm_s));
			
			// Not yet saved
			if((flashAlarms_test_datas.hour == 255) || (flashAlarms_test_datas.min == 255))
				break;
			//if((flashAlarms_test_datas.hour == 0) || (flashAlarms_test_datas.min == 0) || (flashAlarms_test_datas.days == 0))
			//	break;
			
			HAL_Delay(1);
			//memset(flashAlarms, 0x00, sizeof(alarm_s) * ALARM_COUNT);
			W25Q64_Read(FLASH_SECTOR_ALARM, (uint8_t *)flashAlarms, sizeof(alarm_s) * ALARM_COUNT);
			break;
		}
		
		W25Q64_Read(FLASH_SECTOR_ALARM, (uint8_t *)&flash_test_data, 4);
		HAL_Delay(10);
	}
	
	getNextAlarm();

}

void alarm_reset()
{
	memset((uint8_t *)flashAlarms, 0x00, sizeof(alarm_s) * ALARM_COUNT);
	W25Q64_Erase(FLASH_SECTOR_ALARM, 4096);
	W25Q64_Write(FLASH_SECTOR_ALARM, (uint8_t *)flashAlarms, sizeof(alarm_s) * ALARM_COUNT);
}

void alarm_save(byte num, alarm_s* alarm)
{
	memcpy((uint8_t *)&flashAlarms[num], (uint8_t *)alarm, sizeof(alarm_s));
	
	W25Q64_Erase(FLASH_SECTOR_ALARM, 4096);
	W25Q64_Write(FLASH_SECTOR_ALARM, (uint8_t *)flashAlarms, sizeof(alarm_s) * ALARM_COUNT);
	HAL_Delay(100);
	
	getNextAlarm();
}


void alarm_get(byte num, alarm_s* alarm)
{
//	alarm=&flashAlarms[num];
	
	
	
//		ReadToBackupReg(8,(u16*)&flashAlarms,sizeof(flashAlarms));
//	flashAlarms[num]=getAlarmfromBKP(num);
	memcpy(alarm, (uint8_t *)&flashAlarms[num], sizeof(alarm_s));
	
//  	STMFLASH_Read((const u8)&flashAlarms[num],(u32*)alarm,sizeof(alarm_s));

//	eeprom_read_block(alarm, &flashAlarms[num], sizeof(alarm_s));
//	if(alarm->hour > 23)
//		memset(alarm, 0, sizeof(alarm_s));

//	0   1  2  3  4  5 
//	01 23 45 67 89 10 11 12 13

}


bool alarm_getState(void)
{
	return alarmSetOff;
}

bool alarm_getNext(alarm_s* alarm)
{
	if(nextAlarm == NOALARM)
		return false;
	alarm_get(nextAlarm, alarm);
	return true;
}

byte alarm_getNextDay()
{
	return nextAlarmDay;
}



void alarm_update()
{
	bool wasGoingOff = alarmSetOff;
	bool alarmNow = goingOff();
	if(alarmSetOff)
	{
		if(alarmNow)
		{
			if(wasGoingOff != alarmSetOff)
			{
				oldDrawFunc = display_setDrawFunc(draw);
				oldBtn1Func = buttons_setFunc(BTN_1, NULL);
				oldBtn2Func = buttons_setFunc(BTN_2, stopAlarm);
				oldBtn3Func = buttons_setFunc(BTN_3, NULL);
				pwrmgr_setState(PWR_ACTIVE_ALARM, PWR_STATE_IDLE);
				tune_play(tuneAlarm, VOL_ALARM, PRIO_ALARM);
			}
			if(!led_flashing())
			{
				static led_t ledFlash = LED_GREEN;
				ledFlash = (ledFlash == LED_GREEN) ? LED_RED : LED_GREEN;
				led_flash(ledFlash, 150, 255);
			}
		}
		else if(!alarmNow)
			stopAlarm();
	}
}

void alarm_updateNextAlarm()
{
	getNextAlarm();
}

static bool goingOff()
{
	alarm_s nextAlarm;
	
	// Make sure we're in 24 hour mode
	time_s time;
	time.hour = timeDate.time.hour;
	time.ampm = timeDate.time.ampm;
	time_timeMode(&time, TIMEMODE_24HR);

	if(alarm_getNext(&nextAlarm) && alarm_dayEnabled(nextAlarm.days, timeDate.date.day) && nextAlarm.hour == time.hour && nextAlarm.min == timeDate.time.mins)
	{
		if(timeDate.time.secs == 0)
			alarmSetOff = true;
		return true;
	}
	return false;
}

// This func needs to be ran when an alarm has changed, time has changed or an active alarm has been turned off
static void getNextAlarm()
{
	byte next = NOALARM;
	uint nextTime = (uint)UINT_MAX;

	// Make sure time is in 24 hour mode
	time_s timeNow;
	timeNow.hour = timeDate.time.hour;
	timeNow.ampm = timeDate.time.ampm;
	time_timeMode(&timeNow, TIMEMODE_24HR);

	// Now in minutes from start of week
	uint now = toMinutes(timeNow.hour, timeDate.time.mins + 1, timeDate.date.day);

	// Loop through alarms
	LOOPR(ALARM_COUNT, i)
	{
		// Get alarm data
		alarm_s alarm;
		alarm_get(i, &alarm);

		// Not enabled
		if(!alarm.enabled)
			continue;

		// Loop through days
		LOOPR(7, d)
		{
			// Day not enabled
			if(!alarm_dayEnabled(alarm.days, d))
				continue;

			// Alarm time in minutes from start of week
			uint alarmTime = toMinutes(alarm.hour, alarm.min, d);

			// Minutes to alarm
			int timeTo = alarmTime - now;

			// Negative result, must mean alarm time is earlier in the week than now, add a weeks time
			if(timeTo < 0)
				timeTo += ((6*1440) + (23*60) + 59); // 10079

			// Is minutes to alarm less than the last minutes to alarm?
			if((uint)timeTo < nextTime)
			{
				// This is our next alarm
				nextTime = timeTo;
				next = i;
				nextAlarmDay = d;
			}
		}
	}

	// Set next alarm
	nextAlarm = next;
}

static uint toMinutes(byte hours, byte mins, byte dow)
{
	uint total = mins;
	total += hours * 60;
	total += dow * 1440;
	return total;
}

static bool stopAlarm()
{
	getNextAlarm();
	display_setDrawFunc(oldDrawFunc);
	buttons_setFuncs(oldBtn1Func, oldBtn2Func, oldBtn3Func);
	//OLED_ColorTurn(appConfig.invert);
	pwrmgr_setState(PWR_ACTIVE_ALARM, PWR_STATE_NONE);
	tune_stop(PRIO_ALARM);
	alarmSetOff = false;
	return true;
}

static display_t draw()
{
	if((millis8_t)millis() < 128)
		draw_bitmap(16, 16, menu_alarm, 32, 32, NOINVERT, 0);
	
	// Draw time
	draw_string(time_timeStr(), NOINVERT, 79, 20);

	// Draw day
	char buff[BUFFSIZE_STR_DAYS];
	strcpy(buff, days[timeDate.date.day]);
	draw_string(buff, false, 86, 36);

	return DISPLAY_DONE;
}
