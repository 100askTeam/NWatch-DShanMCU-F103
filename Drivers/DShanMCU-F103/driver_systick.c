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


#include "main.h"
#include "driver_systick.h"

/*
 * 全局变量定义
*/

volatile millis_t systick_t = 0;

/*
 *  函数名：void SysTickInit(uint32_t cycle)
 *  输入参数：cycle,设置系统滴答时钟周期
 *  输出参数：无
 *  返回值：无
 *  函数作用：初始化系统滴答时钟的频率和中断优先级
*/
void SysTickInit(uint32_t cycle)
{
    uint32_t init_t = 0;

    init_t = SystemCoreClock/cycle;
  
    /* 时间(单位:s)=1/频率(单位:HZ)
     * SystemCoreClock频率: 72MHz = 72,000,000
     * 即MCU 1秒会计数72,000,000次
     *       1ms则计数 72MHz/1000 = 72000次
     * 72000就是滴答时钟的初始值,它向下计数72000次,计数将变为0,就会产生一次中断
     * 滴答时钟初始值范围:1~16777216
     *
     * SystemCoreClock/1000:    1ms中断一次
     * SystemCoreClock/100000:  10us中断一次
     * SystemCoreClock/1000000: 1us中断一次
     */
    if(HAL_SYSTICK_Config(init_t) != HAL_OK)
    {
        Error_Handler();
    }
    
    // 设置滴答定时器中断优先级：最高
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    // 使能滴答定时器中断
    HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

/*
 *  函数名：void SysTickDelay(uint16_t m)
 *  输入参数：m-延时时间
 *  输出参数：无
 *  返回值：无
 *  函数作用：滴答定时器实现的延时函数
*/
void SysTickDelay(uint32_t m)
{
    //systick_t = m;
    //while(systick_t != 0);
}


uint32_t SysTickGetMs(void)
{
    return systick_t;
}

/*
 *  函数名：void SysTick_Handler(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：滴答定时器的中断服务函数
*/


