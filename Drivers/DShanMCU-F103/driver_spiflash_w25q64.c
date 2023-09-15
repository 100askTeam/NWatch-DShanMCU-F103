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



#include "driver_spiflash_w25q64.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
///#include "adc.h"

extern SPI_HandleTypeDef hspi1;
static SPI_HandleTypeDef *g_HSPI_Flash = &hspi1;

/* CS: PB9
 * SPI1_MISO: PA6
 * SPI1_MOSI: PA7
 * SPI1_SCK:  PA5
 */

#define W25Q64_CS_GPIO_GROUP GPIOB
#define W25Q64_CS_GPIO_PIN   GPIO_PIN_9
#define W25Q64_TIMEOUT       500

/**********************************************************************
 * 函数名称： W25Q64_Select
 * 功能描述： 选中W25Q64
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void W25Q64_Select(void)
{
    HAL_GPIO_WritePin(W25Q64_CS_GPIO_GROUP, W25Q64_CS_GPIO_PIN, GPIO_PIN_RESET);
}

/**********************************************************************
 * 函数名称： W25Q64_Deselect
 * 功能描述： 不选中W25Q64
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void W25Q64_Deselect(void)
{
    HAL_GPIO_WritePin(W25Q64_CS_GPIO_GROUP, W25Q64_CS_GPIO_PIN, GPIO_PIN_SET);
}

/**********************************************************************
 * 函数名称： W25Q64_TxRx
 * 功能描述： 使用SPI发送/接收数据(注意这个函数没有设置片选信号)
 * 输入参数： pTxData - 要发送的数据
 *            Size    - 数据长度
 *            Timeout - 超时时间(单位ms)
 * 输出参数： pRxData - 接收缓冲区
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int  W25Q64_TxRx(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
    if (HAL_OK == HAL_SPI_TransmitReceive(g_HSPI_Flash, pTxData, pRxData, Size, Timeout))
        return 0;
    else
        return -1;
}

/**********************************************************************
 * 函数名称： W25Q64_Tx
 * 功能描述： 使用SPI发送数据(注意这个函数没有设置片选信号)
 * 输入参数： pTxData - 要发送的数据
 *            Size    - 数据长度
 *            Timeout - 超时时间(单位ms)
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int  W25Q64_Tx(uint8_t *pTxData, uint16_t Size, uint32_t Timeout)
{
    if (HAL_OK == HAL_SPI_Transmit(g_HSPI_Flash, pTxData, Size, Timeout))
        return 0;
    else
        return -1;
}

/**********************************************************************
 * 函数名称： W25Q64_Rx
 * 功能描述： 使用SPI读取数据(注意这个函数没有设置片选信号)
 * 输入参数： Size    - 数据长度
 *            Timeout - 超时时间(单位ms)
 * 输出参数： pRxData - 接收缓冲区
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int  W25Q64_Rx(uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
    if (HAL_OK == HAL_SPI_Receive(g_HSPI_Flash, pRxData, Size, Timeout))
        return 0;
    else
        return -1;
}


/**********************************************************************
 * 函数名称： W25Q64_WaitReady
 * 功能描述： 等待W25Q64就绪
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int W25Q64_WaitReady(void)
{
    unsigned char tx_buf[2];
    unsigned char rx_buf[2];
    int timeout = W25Q64_TIMEOUT;

    tx_buf[0] = 0x05; /* 读状态 */
    tx_buf[1] = 0xff;
	
	while (timeout--)
	{
		rx_buf[0] = rx_buf[1] = 0;
        W25Q64_Select();
		W25Q64_TxRx(tx_buf, rx_buf, 2, W25Q64_TIMEOUT);
        W25Q64_Deselect();
		if ((rx_buf[1] & 1) == 0)
			return 0;
        HAL_Delay(1);
	}

    if (!timeout)
    {
    	return -1;
    }
	return 0;
}

/**********************************************************************
 * 函数名称： W25Q64_WriteEnable
 * 功能描述： 写使能
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int W25Q64_WriteEnable(void)
{
    unsigned char tmpbuf[1];
    int err;
    
    tmpbuf[0] = 0x06; /* 写使能 */
    W25Q64_Select();
    err = W25Q64_Tx(tmpbuf, 1, W25Q64_TIMEOUT);
    W25Q64_Deselect();

    return err;
}


/**********************************************************************
 * 函数名称： W25Q64_Init
 * 功能描述： W25Q64的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void W25Q64_Init(void)
{
    /* 片选信号PB9在MX_GPIO_Init中被配置为输出引脚 */
    /* SPI在MX_SPI1_Init中也被配置好了 */
}

/**********************************************************************
 * 函数名称： W25Q64_Read
 * 功能描述： W25Q64读函数
 * 输入参数： offset - 读哪个地址
 *            len    - 读多少字节
 * 输出参数： buf - 用来保存数据
 * 返 回 值： 非负数 - 读取了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int W25Q64_Read(uint32_t offset, uint8_t *buf, uint32_t len)
{
    unsigned char tmpbuf[4];
    int err;
    
    /* 自己实现SPI Flash的读操作 */
    tmpbuf[0] = 0x03;
    tmpbuf[1] = (offset >> 16) & 0xff;
    tmpbuf[2] = (offset >> 8) & 0xff;
    tmpbuf[3] = (offset >> 0) & 0xff;
    
    W25Q64_Select();

    /* 发送读命令 */
    err = W25Q64_Tx(tmpbuf, 4, W25Q64_TIMEOUT);
    if (err)
    {
        W25Q64_Deselect();
        return -1;
    }

    /* 读数据 */
    err = W25Q64_Rx(buf, len, W25Q64_TIMEOUT);
    if (err)
    {
        W25Q64_Deselect();
        return -1;
    }

    W25Q64_Deselect();    
    return len;
}


/**********************************************************************
 * 函数名称： W25Q64_Write
 * 功能描述： W25Q64写函数(需要先擦除)
 * 输入参数： offset - 写哪个地址
 *            buf    - 数据buffer
 *            len    - 写多少字节
 * 输出参数： 无
 * 返 回 值： 非负数 - 写了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int W25Q64_Write(uint32_t offset, uint8_t *buf, uint32_t len)
{
    uint8_t tmpbuf[4];
    uint32_t phy_pos = offset;
    int err;
    uint32_t cur_len;
    uint32_t remain_len;

    /* 写数据 */
    phy_pos = offset;
    remain_len = len;

    /* 可能不是从Page开头写数据(1个Page是256字节) */
    cur_len = offset & (256-1);
    cur_len = 256 - cur_len;
    if (cur_len > len)
        cur_len = len;
    
    for (; phy_pos < offset + len; )
    {
        /* 写使能 */
        err = W25Q64_WriteEnable();
        if (err)
        {
            return -1;
        }
        
        tmpbuf[0] = 0x02; /* page program */
        tmpbuf[1] = (phy_pos >> 16) & 0xff;
        tmpbuf[2] = (phy_pos >> 8) & 0xff;
        tmpbuf[3] = (phy_pos >> 0) & 0xff;

        /* 发送page program命令 */
        W25Q64_Select();
        err = W25Q64_Tx(tmpbuf, 4, W25Q64_TIMEOUT);
        if (err)
        {
            W25Q64_Deselect();
            return -1;
        }
        
        /* 发送数据 */
        err = W25Q64_Tx(buf, cur_len, W25Q64_TIMEOUT);
        if (err)
        {
            W25Q64_Deselect();
            return -1;
        }
        W25Q64_Deselect();
        
        /* 读状态 */
        err = W25Q64_WaitReady();
        if (err)
        {
            return -1;
        }

        phy_pos += cur_len;
        buf     += cur_len;
        remain_len -= cur_len;

        cur_len = (remain_len < 256) ? remain_len : 256;
    }
    return len;
}

/**********************************************************************
 * 函数名称： W25Q64_Erase
 * 功能描述： W25Q64写函数
 * 输入参数： offset - 擦除哪个地址(4096对齐)
 *            len    - 擦除多少字节(4096对齐)
 * 输出参数： 无
 * 返 回 值： 非负数 - 擦除了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int W25Q64_Erase(uint32_t offset, uint32_t len)
{
    unsigned char tmpbuf[4];
    uint32_t phy_pos = offset;
    int err;

    if ((offset & (4096-1)) && (len & (4096-1)))
        return -1;

    for (int sector = 0; sector < len/4096; sector++)
    {
        /* 写使能 */
        err = W25Q64_WriteEnable();
        if (err)
        {
            return -1;
        }
        
        tmpbuf[0] = 0x20; /* 擦除 */
        tmpbuf[1] = (phy_pos >> 16) & 0xff;
        tmpbuf[2] = (phy_pos >> 8) & 0xff;
        tmpbuf[3] = (phy_pos >> 0) & 0xff;
        
        W25Q64_Select();
        W25Q64_Tx(tmpbuf, 4, W25Q64_TIMEOUT);
        if (err)
        {
            W25Q64_Deselect();
            return -1;
        }
        W25Q64_Deselect();
        
        phy_pos += 4096;

        /* 读状态 */
        err = W25Q64_WaitReady();
        if (err)
        {
            return -1;
        }
    }

    return len;
}


/**********************************************************************
 * 函数名称： W25Q64_Test
 * 功能描述： W25Q64测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void W25Q64_Test(void)
{
    int sector;
    int len;
    uint8_t buf[4];
    int i;
    uint32_t val1, val2;
    
    W25Q64_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "W25Q64 Test: ");

        for (sector = 0; sector < 2048; sector++)
        {
            /* 擦除测试 */
            LCD_ClearLine(0, 2);
            len = LCD_PrintString(0, 2, "Erase ");
            LCD_PrintSignedVal(len, 2, sector);
            W25Q64_Erase(sector * 4096, 4096);            
            W25Q64_Read(sector * 4096, buf, 4);
            LCD_ClearLine(0, 4);
            for (i = 0; i < 4; i++)
            {
                LCD_PrintHex(i*3, 4, buf[i], 0);
            }
            HAL_Delay(1000);

            /* 写入测试 */
            LCD_ClearLine(0, 2);
            len = LCD_PrintString(0, 2, "Write ");
            LCD_PrintSignedVal(len, 2, sector);

            val1 = system_get_ns();
            W25Q64_Write(sector * 4096, (uint8_t *)&val1, 4);
            LCD_ClearLine(0, 4);
            LCD_PrintHex(0, 4, val1, 1);
            HAL_Delay(1000);

            /* 读出测试 */
            LCD_ClearLine(0, 2);
            len = LCD_PrintString(0, 2, "Read ");
            LCD_PrintSignedVal(len, 2, sector);

            W25Q64_Read(sector * 4096, (uint8_t *)&val2, 4);
            LCD_ClearLine(0, 4);
            if (val1 == val2)
                LCD_PrintString(0, 4, "Test ok");
            else
            {
                LCD_PrintHex(0, 4, val2, 1);
                LCD_ClearLine(0, 6);
                LCD_PrintString(0, 6, "Test Failed");
            }
            HAL_Delay(1000);
            
        }
    }
}


