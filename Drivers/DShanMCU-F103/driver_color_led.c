// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */
 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 * 免责声明: 百问网编写的文档, 仅供学员学习使用, 可以转发或引用(请保留作者信息),禁止用于商业用途！
 * 免责声明: 百问网编写的程序, 可以用于商业用途, 但百问网不承担任何后果！
 * 
 * 文件名称：driver_color_led.c
 * 本程序遵循GPL V3协议, 请遵循协议
 * 百问网学习平台   : https://www.100ask.net
 * 百问网交流社区   : https://forums.100ask.net
 * 百问网官方B站    : https://space.bilibili.com/275908810
 * 本程序所用开发板 : DShanMCU-F103
 * 百问网官方淘宝   : https://100ask.taobao.com
 * 联系我们(E-mail): weidongshan@qq.com
 *
 *          版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
 */

#include "driver_color_led.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

#define CHANNEL_RED   TIM_CHANNEL_3
#define CHANNEL_GREEN TIM_CHANNEL_1
#define CHANNEL_BLUE  TIM_CHANNEL_2

typedef struct {
	millis_t flashLen;			// How long to light up for
	millis_t startTime;
	uint32_t color;
}led_s;

static led_s g_tledRed;
static led_s g_tledGreen;
static led_s g_tledBlue;

static BOOL _update(led_s* led);
static void flash(led_s* led, millis_t len, uint32_t brightness);

extern TIM_HandleTypeDef htim2;

static TIM_HandleTypeDef *g_HPWM_ColorLED = &htim2;

/**********************************************************************
 * 函数名称： ColorLED_Init
 * 功能描述： 全彩LED的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ColorLED_Init(void)
{
  ///MX_TIM2_Init();
	ColorLED_Set(COLOR_LED_GREEN);
}

/**********************************************************************
 * 函数名称： ColorLED_Set
 * 功能描述： 全彩LED设置颜色函数
 * 输入参数： color - 24bit颜色,格式为0x00RRGGBB
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ColorLED_Set(uint32_t color)
{
    TIM_OC_InitTypeDef sConfigR = {0};
    TIM_OC_InitTypeDef sConfigG = {0};
    TIM_OC_InitTypeDef sConfigB = {0};

    int r,g,b;

    r = (color >> 16) & 0xff;
    g = (color >> 8) & 0xff;
    b = (color >> 0) & 0xff;

    sConfigR.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfigR.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfigR.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfigR.Pulse = r*2000/255;              // 在 PWM1 模式下,通道 3(RLED)占空比

    sConfigG.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfigG.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfigG.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfigG.Pulse = g*2000/255;              // 在 PWM1 模式下,通道 3(RLED)占空比

    sConfigB.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfigB.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfigB.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfigB.Pulse = b*2000/255;              // 在 PWM1 模式下,通道 3(RLED)占空比

    HAL_TIM_PWM_Stop(g_HPWM_ColorLED, CHANNEL_RED);
    HAL_TIM_PWM_Stop(g_HPWM_ColorLED, CHANNEL_GREEN);
    HAL_TIM_PWM_Stop(g_HPWM_ColorLED, CHANNEL_BLUE);
    
    HAL_TIM_PWM_ConfigChannel(g_HPWM_ColorLED, &sConfigR, CHANNEL_RED);
    HAL_TIM_PWM_ConfigChannel(g_HPWM_ColorLED, &sConfigG, CHANNEL_GREEN);
    HAL_TIM_PWM_ConfigChannel(g_HPWM_ColorLED, &sConfigB, CHANNEL_BLUE);

    HAL_TIM_PWM_Start(g_HPWM_ColorLED, CHANNEL_RED);
    HAL_TIM_PWM_Start(g_HPWM_ColorLED, CHANNEL_GREEN);
    HAL_TIM_PWM_Start(g_HPWM_ColorLED, CHANNEL_BLUE);    
}



// Turn off LEDs
void led_stop(void)
{
	ColorLED_Set(0x00000000);
	led_update();
}

void led_flash(led_t led, millis_t len, uint32_t brightness)
{
	if (!appConfig.CTRL_LEDs) 
	{
		switch(led)
		{
			case LED_RED:
				g_tledRed.color = COLOR_LED_RED;
				flash(&g_tledRed, len, brightness);
				break;
			case LED_GREEN:
				g_tledGreen.color = COLOR_LED_GREEN;
				flash(&g_tledGreen, len, brightness);
				break;
			case LED_BLUE:
				g_tledBlue.color = COLOR_LED_BLUE;
				flash(&g_tledBlue, len, brightness);
				break;
			default:
				break;
		}

		pwrmgr_setState(PWR_ACTIVE_LED, PWR_STATE_IDLE);
	}
	else
			ColorLED_Set(0x00000000);
}

// Is an LED on?
BOOL led_flashing(void)
{
	return g_tledRed.flashLen || g_tledGreen.flashLen;
}



void led_update(void)
{
	BOOL red = _update(&g_tledRed);
	BOOL green = _update(&g_tledGreen);

	if(!red && !green)
	{
		// Timer no longer in use, disable
		///power_timer0_disable();
		pwrmgr_setState(PWR_ACTIVE_LED, PWR_STATE_NONE);
	}
}



static BOOL _update(led_s* led)
{
	if(led->flashLen && ((millis() - led->startTime) >= led->flashLen))
	{
		ColorLED_Set(0x00000000);
		led->flashLen = 0;
	}
	else	ColorLED_Set(led->color);

	return led->flashLen;
}

static void flash(led_s* led, millis_t len, uint32_t brightness)
{
	UNUSED(brightness);
	led->flashLen = len;
	led->startTime = millis();

	///if(brightness == 255 || brightness == 0)
	///{
	///	TCCR0A &= ~com;
	///	brightness == 255 ? (PORTD |= pin) : (PORTD &= ~pin);
	///}
	///else
	///{
	///	power_timer0_enable();
	///	TCCR0A |= com;
	///	*ocr = brightness;
	///}
}


/**********************************************************************
 * 函数名称： ColorLED_Test
 * 功能描述： 全彩LED测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void ColorLED_Test(void)
{
    uint32_t color = 0;

    ///ColorLED_Init();

    while (1)
    {
        //LCD_PrintString(0, 0, "Show Color: ");
        //LCD_PrintHex(0, 2, color, 1);
        
        ColorLED_Set(0x00000000);
				HAL_Delay(2000);

        color += 200000;
        color &= 0x00ffffff;
				ColorLED_Set(color);
        HAL_Delay(1000);
    }    
}

