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



#include "driver_ir_receiver.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

/* 环形缓冲区: 用来保存解析出来的按键,可以防止丢失 */
#define BUF_LEN 128
static unsigned char g_KeysBuf[BUF_LEN];
static int g_KeysBuf_R, g_KeysBuf_W;

static uint64_t g_IRReceiverIRQ_Timers[68];
static int g_IRReceiverIRQ_Cnt = 0;

#define NEXT_POS(x) ((x+1) % BUF_LEN)

/* 辅助函数 */

/**********************************************************************
 * 函数名称： isKeysBufEmpty
 * 功能描述： 环形缓冲区是否空
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-空, 0-非空
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isKeysBufEmpty(void)
{
	return (g_KeysBuf_R == g_KeysBuf_W);
}

/**********************************************************************
 * 函数名称： isKeysBufFull
 * 功能描述： 环形缓冲区是否满
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-满, 0-未满
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isKeysBufFull(void)
{
	return (g_KeysBuf_R == NEXT_POS(g_KeysBuf_W));
}

/**********************************************************************
 * 函数名称： PutKeyToBuf
 * 功能描述： 把按键数据写入环形缓冲区
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void PutKeyToBuf(unsigned char key)
{
	if (!isKeysBufFull())
	{
		g_KeysBuf[g_KeysBuf_W] = key;
		g_KeysBuf_W = NEXT_POS(g_KeysBuf_W);
	}
}

/**********************************************************************
 * 函数名称： GetKeyFromBuf
 * 功能描述： 从环形缓冲区读取按键数据
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0xff - 读不到数据, 其他值-按键值(device或key)
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static unsigned char GetKeyFromBuf(void)
{
	unsigned char key = 0xff;
	if (!isKeysBufEmpty())
	{
		key = g_KeysBuf[g_KeysBuf_R];
		g_KeysBuf_R = NEXT_POS(g_KeysBuf_R);
	}
	return key;
}

/**********************************************************************
 * 函数名称： IRReceiver_IRQTimes_Parse
 * 功能描述： 解析中断回调函数里记录的时间序列,得到的device和key放入环形缓冲区
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int IRReceiver_IRQTimes_Parse(void)
{
	uint64_t time;
	int i;
	int m, n;
	unsigned char datas[4];
	unsigned char data = 0;
	int bits = 0;
	int byte = 0;

	/* 1. 判断前导码 : 9ms的低脉冲, 4.5ms高脉冲  */
	time = g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0];
	if (time < 8000000 || time > 10000000)
	{
		return -1;
	}

	time = g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1];
	if (time < 3500000 || time > 55000000)
	{
		return -1;
	}

	/* 2. 解析数据 */
	for (i = 0; i < 32; i++)
	{
		m = 3 + i*2;
		n = m+1;
		time = g_IRReceiverIRQ_Timers[n] - g_IRReceiverIRQ_Timers[m];
		data <<= 1;
		bits++;
		if (time > 1000000)
		{
			/* 得到了数据1 */
			data |= 1;
		}

		if (bits == 8)
		{
			datas[byte] = data;
			byte++;
			data = 0;
			bits = 0;
		}
	}

	/* 判断数据正误 */
	datas[1] = ~datas[1];
	datas[3] = ~datas[3];
	
	if ((datas[0] != datas[1]) || (datas[2] != datas[3]))
	{
        g_IRReceiverIRQ_Cnt = 0;
        return -1;
	}

	PutKeyToBuf(datas[0]);
	PutKeyToBuf(datas[2]);
    return 0;
}


/**********************************************************************
 * 函数名称： isRepeatedKey
 * 功能描述： 解析中断回调函数里记录的时间序列,判断是否重复码
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1 - 是, (0) - 不是
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isRepeatedKey(void)
{
	uint64_t time;

	/* 1. 判断重复码 : 9ms的低脉冲, 2.25ms高脉冲  */
	time = g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0];
	if (time < 8000000 || time > 10000000)
	{
		return 0;
	}

	time = g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1];
	if (time < 2000000 || time > 2500000)
	{
		return 0;
	}	

	return 1;
}

/**********************************************************************
 * 函数名称： IRReceiver_IRQ_Callback
 * 功能描述： 红外接收器的中断回调函数,记录中断时刻
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void IRReceiver_IRQ_Callback(void)
{
    uint64_t time;
    static uint64_t pre_time = 0;

        
	/* 1. 记录中断发生的时刻 */	
	time = system_get_ns();
    
    /* 一次按键的最长数据 = 引导码 + 32个数据"1" = 9+4.5+2.25*32 = 85.5ms
     * 如果当前中断的时刻, 举例上次中断的时刻超过这个时间, 以前的数据就抛弃
     */
    if (time - pre_time > 100000000) 
    {
        g_IRReceiverIRQ_Cnt = 0;
    }
    pre_time = time;
    
	g_IRReceiverIRQ_Timers[g_IRReceiverIRQ_Cnt] = time;

	/* 2. 累计中断次数 */
	g_IRReceiverIRQ_Cnt++;

	/* 3. 次数达标后, 解析数据, 放入buffer */
	if (g_IRReceiverIRQ_Cnt == 4)
	{
		/* 是否重复码 */
		if (isRepeatedKey())
		{
			/* device: 0, val: 0, 表示重复码 */
			PutKeyToBuf(0);
			PutKeyToBuf(0);
			g_IRReceiverIRQ_Cnt = 0;
		}
	}
	if (g_IRReceiverIRQ_Cnt == 68)
	{
		IRReceiver_IRQTimes_Parse();
		g_IRReceiverIRQ_Cnt = 0;
	}
}

/**********************************************************************
 * 函数名称： IRReceiver_Init
 * 功能描述： 红外接收器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void IRReceiver_Init(void)
{
    /* PA10在MX_GPIO_Init()中已经被配置为双边沿触发, 并使能了中断 */
#if 0
    /*Configure GPIO pin : PB10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
}

/**********************************************************************
 * 函数名称： IRReceiver_Read
 * 功能描述： 红外接收器的读取函数
 * 输入参数： 无
 * 输出参数： pDev  - 用来保存设备ID
 *            pData - 用来保存按键码
 * 返 回 值： 0 - 成功, (-1) - 失败(无数据)
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int IRReceiver_Read(uint8_t *pDev, uint8_t *pData)
{
    if (isKeysBufEmpty())
        return -1;
    
    *pDev  = GetKeyFromBuf();
    *pData = GetKeyFromBuf();
    return 0;
}

/**********************************************************************
 * 函数名称： IRReceiver_CodeToString
 * 功能描述： 把接收到的按键码转换为按键名字
 * 输入参数： code - 按键码
 * 输出参数： 无
 * 返 回 值： NULL - 未识别的按键码; 其他值 - 按键名字
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
const char *IRReceiver_CodeToString(uint8_t code)
{
    const uint8_t codes[]= {0xa2, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, \
                            0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, \
                            0x5a, 0x42, 0x4a, 0x52, 0x00};
    const char *names[]= {"Power", "Menu", "Test", "+", "Return", "Left", "Play", "Right", \
                            "0", "-", "C", "1", "2", "3", "4", "5", \
                            "6", "7", "8", "9", "Repeat"};
    int i;
    
    for (i = 0; i < sizeof(codes)/sizeof(codes[0]); i++)
    {
        if (code == codes[i])
        {
            return names[i];
        }
    }
    return "Error";
}


/**********************************************************************
 * 函数名称： IRReceiver_Test
 * 功能描述： 红外接收器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void IRReceiver_Test(void)
{
    uint8_t dev, data;
    int len;
	
    IRReceiver_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "IR Receiver: ");        
        LCD_PrintString(0, 2, "Device  Data");

        if (!IRReceiver_Read(&dev, &data))
        {
            LCD_PrintString(0, 4, "                ");
            LCD_PrintHex(0, 4, dev, 1);
            LCD_PrintHex(8, 4, data, 1);
            LCD_PrintString(0, 6, "                ");
            len = LCD_PrintString(0, 6, "Key name: ");
            LCD_PrintString(len, 6, IRReceiver_CodeToString(data));
        }
    }
}


