// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */
 
/*  Copyright (C) 2008-2023 ???????????
 *  All rights reserved
 *
 * ????: ????????, ????????, ???????(???????),????????!
 * ????: ????????, ????????, ???????????!
 * 
 * ????:driver_color_led.c
 * ?????GPL V3??, ?????
 * ???????   : https://www.100ask.net
 * ???????   : https://forums.100ask.net
 * ?????B?    : https://space.bilibili.com/275908810
 * ???????? : DShanMCU-F103
 * ???????   : https://100ask.taobao.com
 * ????(E-mail): weidongshan@qq.com
 *
 *          ????,?????
 *  
 * ????     ???           ??        ????
 *-----------------------------------------------------
 * 2023.08.04      v01         ????      ????
 *-----------------------------------------------------
 */

#include "driver_ir_receiver.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

/* 环形缓冲�? 用来保存解析出来的按�?可以防止丢失 */
#define BUF_LEN 128
static unsigned char g_KeysBuf[BUF_LEN];
static int g_KeysBuf_R, g_KeysBuf_W;

static uint64_t g_IRReceiverIRQ_Timers[68];
static int g_IRReceiverIRQ_Cnt = 0;

#define NEXT_POS(x) ((x+1) % BUF_LEN)

/* 辅助函数 */

/**********************************************************************
 * 函数名称�?isKeysBufEmpty
 * 功能描述�?环形缓冲区是否空
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： 1-�? 0-非空
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
 ***********************************************************************/
static int isKeysBufEmpty(void)
{
	return (g_KeysBuf_R == g_KeysBuf_W);
}

/**********************************************************************
 * 函数名称�?isKeysBufFull
 * 功能描述�?环形缓冲区是否满
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： 1-�? 0-未满
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
 ***********************************************************************/
static int isKeysBufFull(void)
{
	return (g_KeysBuf_R == NEXT_POS(g_KeysBuf_W));
}

/**********************************************************************
 * 函数名称�?PutKeyToBuf
 * 功能描述�?把按键数据写入环形缓冲区
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： �
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
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
 * 函数名称�?GetKeyFromBuf
 * 功能描述�?从环形缓冲区读取按键数据
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： 0xff - 读不到数�? 其他�?按键�?device或key)
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
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
 * 函数名称�?IRReceiver_IRQTimes_Parse
 * 功能描述�?解析中断回调函数里记录的时间序列,得到的device和key放入环形缓冲�
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： 0 - 成功, (-1) - 失败
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
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

	/* 1. 判断前导�?: 9ms的低脉冲, 4.5ms高脉�? */
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
			/* 得到了数�? */
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
 * 函数名称�?isRepeatedKey
 * 功能描述�?解析中断回调函数里记录的时间序列,判断是否重复�
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： 1 - �? (0) - 不是
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
 ***********************************************************************/
static int isRepeatedKey(void)
{
	uint64_t time;

	/* 1. 判断重复�?: 9ms的低脉冲, 2.25ms高脉�? */
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
 * 函数名称�?IRReceiver_IRQ_Callback
 * 功能描述�?红外接收器的中断回调函数,记录中断时刻
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： �
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
 ***********************************************************************/
void IRReceiver_IRQ_Callback(void)
{
    uint64_t time;
    static uint64_t pre_time = 0;

        
	/* 1. 记录中断发生的时�?*/	
	time = system_get_ns();
    
    /* 一次按键的最长数�?= 引导�?+ 32个数�?1" = 9+4.5+2.25*32 = 85.5ms
     * 如果当前中断的时�? 举例上次中断的时刻超过这个时�? 以前的数据就抛弃
     */
    if (time - pre_time > 100000000) 
    {
        g_IRReceiverIRQ_Cnt = 0;
    }
    pre_time = time;
    
	g_IRReceiverIRQ_Timers[g_IRReceiverIRQ_Cnt] = time;

	/* 2. 累计中断次数 */
	g_IRReceiverIRQ_Cnt++;

	/* 3. 次数达标�? 解析数据, 放入buffer */
	if (g_IRReceiverIRQ_Cnt == 4)
	{
		/* 是否重复�?*/
		if (isRepeatedKey())
		{
			/* device: 0, val: 0, 表示重复�?*/
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
 * 函数名称�?IRReceiver_Init
 * 功能描述�?红外接收器的初始化函�
 * 输入参数�?�
 * 输出参数�?�
 * �?�?值： �
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
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
 * 函数名称�?IRReceiver_Read
 * 功能描述�?红外接收器的读取函数
 * 输入参数�?�
 * 输出参数�?pDev  - 用来保存设备ID
 *            pData - 用来保存按键�
 * �?�?值： 0 - 成功, (-1) - 失败(无数�?
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
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
 * 函数名称�?IRReceiver_CodeToString
 * 功能描述�?把接收到的按键码转换为按键名�
 * 输入参数�?code - 按键�
 * 输出参数�?�
 * �?�?值： NULL - 未识别的按键�? 其他�?- 按键名字
 * 修改日期�?     版本�?    修改�?      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东�?      创建
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
 * 函数名称�?IRReceiver_Test
 * 功能描述�?红外接收器测试程�
 * 输入参数�?�
 * 输出参数�?�
 *            �
 * �?�?值： �
 * 修改日期        版本�?    修改�?       修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东�?      创建
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
