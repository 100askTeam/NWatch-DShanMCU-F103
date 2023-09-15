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

#include "driver_dht11.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"

static dht11_t  g_dht11_data_t;

/* 控制GPIO读取DHT11的数据 
 * 1. 主机发出至少18MS的低脉冲: start信号
 * 2. start信号变为高, 20-40us之后, dht11会拉低总线维持80us
      然后拉高80us: 回应信号
 * 3. 之后就是数据, 逐位发送
 *    bit0 : 50us低脉冲, 26-28us高脉冲
 *    bit1 : 50us低脉冲, 70us高脉冲
 * 4. 数据有40bit: 8bit湿度整数数据+8bit湿度小数数据
                   +8bit温度整数数据+8bit温度小数数据
                   +8bit校验和
 */


/* 先实现GPIO的基本操作 */
/**********************************************************************
 * 函数名称： DHT11_PinCfgAsOutput
 * 功能描述： 把DHT11的数据引脚配置为输出
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_PinCfgAsOutput(void)
{
    /* 对于STM32F103, 已经把DHT11的引脚配置为"open drain, pull-up" */
}

/**********************************************************************
 * 函数名称： DHT11_PinCfgAsInput
 * 功能描述： 把DHT11的数据引脚配置为输入
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_PinCfgAsInput(void)
{
    /* 对于STM32F103, 已经把DHT11的引脚配置为"open drain, pull-up" 
	* 让它输出1就不会驱动这个引脚, 并且可以读入引脚状态
     */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

/**********************************************************************
 * 函数名称： DHT11_PinSet
 * 功能描述： 设置DHT11的数据引脚的输出值
 * 输入参数： val - 输出电平
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_PinSet(int val)
{
	if (val)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

/**********************************************************************
 * 函数名称： DHT11_PinRead
 * 功能描述： 读取DHT11的数据引脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-高电平, 0-低电平
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_PinRead(void)
{
    if (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))
		return 1;
	else
		return 0;
}


/* 再来实现DHT11的读操作 */
/**********************************************************************
 * 函数名称： DHT11_Start
 * 功能描述： 给DHT11发出启动信号 
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_Start(void)
{
	DHT11_PinSet(0);
	mdelay(20);
	DHT11_PinCfgAsInput();
}


/**********************************************************************
 * 函数名称： DHT11_Wait_Ack
 * 功能描述： 等待DHT11的回应信号
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-无响应, 0-有响应
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_Wait_Ack(void)
{
	udelay(60);
	return DHT11_PinRead();
}

/**********************************************************************
 * 函数名称： DHT11_WaitFor_Val
 * 功能描述： 在指定时间内等待数据引脚变为某个值
 * 输入参数： val - 期待数据引脚变为这个值
 *            timeout_us - 超时时间(单位us)
 * 输出参数： 无
 * 返 回 值： 0-成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_WaitFor_Val(int val, int timeout_us)
{
	while (timeout_us--)
	{
		if (DHT11_PinRead() == val)
			return 0; /* ok */
		udelay(1);
	}
	return -1; /* err */
}

/**********************************************************************
 * 函数名称： DHT11_ReadByte
 * 功能描述： 读取DH11 1byte数据
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 数据
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_ReadByte(void)
{
	int i;
	int data = 0;
	
	for (i = 0; i < 8; i++)
	{
		if (DHT11_WaitFor_Val(1, 1000))
		{
			//printf("dht11 wait for high data err!\n\r");
			return -1;
		}
		udelay(40);
		data <<= 1;
		if (DHT11_PinRead() == 1)
			data |= 1;
		
		if (DHT11_WaitFor_Val(0, 1000))
		{
			//printf("dht11 wait for low data err!\n\r");
			return -1;
		}
	}
	
	return data;
}



/* 公开的函数 */

/**********************************************************************
 * 函数名称： DHT11_Init
 * 功能描述： DHT11的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void DHT11_Init(void)
{
	DHT11_PinCfgAsOutput();
	DHT11_PinSet(1);
	//mdelay(2000);
}


/**********************************************************************
 * 函数名称： DHT11_Read
 * 功能描述： 读取DHT11的温度/湿度
 * 输入参数： 无
 * 输出参数： hum  - 用于保存湿度值
 *            temp - 用于保存温度值
 * 返 回 值： 0 - 成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int DHT11_Read(int *hum, int *temp)
{
	unsigned char hum_m, hum_n;
	unsigned char temp_m, temp_n;
	unsigned char check;	
	
	DHT11_Start();
	
	if (0 != DHT11_Wait_Ack())
	{
		//printf("dht11 not ack, err!\n\r");
		return -1;
	}

	if (0 != DHT11_WaitFor_Val(1, 1000))  /* 等待ACK变为高电平, 超时时间是1000us */
	{
		//printf("dht11 wait for ack high err!\n\r");
		return -1;
	}

	if (0 != DHT11_WaitFor_Val(0, 1000))  /* 数据阶段: 等待低电平, 超时时间是1000us */
	{
		//printf("dht11 wait for data low err!\n\r");
		return -1;
	}

	hum_m  = DHT11_ReadByte();
	hum_n  = DHT11_ReadByte();
	temp_m = DHT11_ReadByte();
	temp_n = DHT11_ReadByte();
	check  = DHT11_ReadByte();

	DHT11_PinCfgAsOutput();
	DHT11_PinSet(1);

	if (hum_m + hum_n + temp_m + temp_n == check)
	{
		*hum  = hum_m;
		*temp = temp_m;
		return 0;
	}
	else
	{
		//printf("dht11 checksum err!\n\r");
		return -1;
	}

}

void DHT11_Update(void)
{
	static millis_t lastUpdate = 0;

  // Update every 10ms
  millis_t now = millis();
  if ((millis_t)(now - lastUpdate) >= 100)
  {
    lastUpdate = now;
		DHT11_Read(&g_dht11_data_t.hum, &g_dht11_data_t.temp);
  }
}	


dht11_t *DHT11_Get_Data(void)
{
	return &g_dht11_data_t;
}

/**********************************************************************
 * 函数名称： DHT11_Test
 * 功能描述： DHT11测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void DHT11_Test(void)
{
	int hum, temp;
    int len;
	
	DHT11_Init();
	
	while (1)
	{
		if (DHT11_Read(&hum, &temp) !=0 )
		{
			//printf("\n\rdht11 read err!\n\r");
			DHT11_Init();
		}
		else
		{
			LCD_PrintString(0, 0, "DHT11:");
            LCD_PrintString(0, 2, "Humidity Temp");
            len = LCD_PrintSignedVal(0, 4, hum);
            LCD_PutChar(len, 4, '%');
            LCD_PrintSignedVal(9, 4, temp);
		}
		mdelay(2000);  /* 读取周期是2S, 不能读太频繁 */
	}
}



