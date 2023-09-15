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

#ifndef __DRIVER_SYSTICK_H
#define __DRIVER_SYSTICK_H

#include "stm32f1xx_hal.h"

#define CYCLE_100MS   10
#define CYCLE_10MS    100
#define CYCLE_1MS     1000 
#define CYCLE_100US   10000
#define CYCLE_10US    100000
#define CYCLE_1US     1000000

/*
 *  函数名：void SysTickInit(void)
 *  输入参数：cycle,设置系统滴答时钟周期
 *  输出参数：无
 *  返回值：无
 *  函数作用：初始化系统滴答时钟的频率和中断优先级
*/
extern void SysTickInit(uint32_t cycle);

/*
 *  函数名：void SysTickDelay(uint16_t m)
 *  输入参数：m-延时时间
 *  输出参数：无
 *  返回值：无
 *  函数作用：滴答定时器实现的延时函数
*/
extern void SysTickDelay(uint32_t m);

millis_t SysTickGetMs(void);

extern volatile uint32_t systick_t;

#endif


