/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/ 
 */
//#include "delay.h"
#include "common.h"

appconfig_s appConfig;     //appconfig_s的长度为8


void appconfig_init()
{
	uint8_t try_count = 3;
	uint32_t flash_test_data = 0;
	
	W25Q64_Read(FLASH_SECTOR_APP_CONFIG, (uint8_t *)&flash_test_data, 4);
	while(try_count--)
	{
		if((flash_test_data != 0) || (flash_test_data != 0xff))
		{
			HAL_Delay(10);
			memset(&appConfig, 0x00, sizeof(appconfig_s));
			W25Q64_Read(FLASH_SECTOR_APP_CONFIG, (uint8_t *)&appConfig, sizeof(appconfig_s));
			return;
		}
		else
			W25Q64_Read(FLASH_SECTOR_APP_CONFIG, (uint8_t *)&flash_test_data, 4);
		
		HAL_Delay(30);
	}

	appconfig_reset();
}

void appconfig_save()
{			   
	///eeprom_update_block(&appConfig, &eepConfig, sizeof(appconfig_s));
	W25Q64_Erase(FLASH_SECTOR_APP_CONFIG, 4096);
	W25Q64_Write(FLASH_SECTOR_APP_CONFIG, (uint8_t *)&appConfig, sizeof(appconfig_s));
}

void appconfig_reset()
{
	appConfig.sleepTimeout = 1;
	appConfig.invert = false;
#if COMPILE_ANIMATIONS
	appConfig.animations = true;
#endif
	appConfig.display180 = false;
  appConfig.CTRL_LEDs = false;
	appConfig.showFPS = false;
	appConfig.timeMode = TIMEMODE_24HR;
	appConfig.volumes = 255;
	appConfig.brightness = 3;
	
	appconfig_save();

	alarm_reset();
}
