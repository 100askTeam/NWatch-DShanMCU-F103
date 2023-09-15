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

#include "driver_oled.h"
#include "ascii_font.c"
#include <string.h>

#include "stm32f1xx_hal.h"

static uint8_t oledBuffer[FRAME_BUFFER_SIZE];

/*
    全屏点亮或者熄灭
*/
#define ENTIRE_DISP_ON()       OLED_WriteCmd(0xA5) 
#define ENTIRE_DISP_OFF()      OLED_WriteCmd(0xA4) 
/*
    阴码显示或者阳码显示
*/
#define DISP_NORMAL()          OLED_WriteCmd(0xA6)  
#define DISP_INVERSE()         OLED_WriteCmd(0xA7)
/*
    打开显示或者关闭显示
*/
#define DISP_ON()              OLED_WriteCmd(0xAF) 
#define DISP_OFF()             OLED_WriteCmd(0xAE) 

/* 2. 滚动命令功能函数 */
typedef enum
{
    H_RIGHT     = 0x26,
    H_LEFT      = 0x27,
}H_SCROLL_DIR;  // 水平滚动方向


typedef enum
{
    HV_RIGHT    = 0x29,
    HV_LEFT     = 0x2A,
}HV_SCROLL_DIR;     // 水平和垂直滚动的方向


/* 
    开始或者停止滚动
*/
#define SCROLL_ON()             OLED_WriteCmd(0x2F)
#define SCROLL_OFF()            OLED_WriteCmd(0x2E)


/* 3. 地址设置功能函数 */
typedef enum
{
    H_ADDR_MODE     = 0,    // 水平地址模式
    V_ADDR_MODE     = 1,    // 垂直地址模式
    PAGE_ADDR_MODE  = 2,    // 页地址模式
}MEM_MODE;  // 内存地址模式


/*
    行地址翻转或者不翻转
*/
#define OLED_SEG_REMAP()        OLED_WriteCmd(0xA1)
#define OLED_SEG_NOREMAP()      OLED_WriteCmd(0xA0)


/*
    COM引脚扫描方向正常扫描或者逆扫描
*/
#define OLED_SCAN_NORMAL()      OLED_WriteCmd(0xC0)
#define OLED_SCAN_REMAP()       OLED_WriteCmd(0xC8)


typedef enum
{
    COM_PIN_SEQ     = 0,
    COM_PIN_ALT     = 1,
}COM_PINS_MODE; // COM引脚属性
typedef enum
{
    COM_NOREMAP     = 0,
    COM_REMAP       = 1,
}COM_REMAP_STATE;   // COM引脚翻转

    
typedef enum
{
    LEVEL_0     = 0x00,
    LEVEL_1     = 0x20,
    LEVEL_2     = 0x30,
}VCOMH_LEVEL;   // 电压等级

/* 6. 电荷碰撞功能函数 */
typedef enum
{
    PUMP_DISABLE    = 0,
    PUMP_ENABLE     = 1,
}CHARGE_PUMP_STATE; // 打开或者关闭电荷泵


#define OELD_I2C_ADDR 0x78
#define OLED_TIMEOUT  500
extern I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef *g_pHI2COLED = &hi2c1;

/*
 *  函数名：OLED_WriteCmd
 *  功能描述：I2C发送命令给OLED
 *  输入参数：cmd-发送给OLED的命令
 *  输出参数：无
 *  返回值：0-成功, 其他值失败
 */
static int OLED_WriteCmd(uint8_t cmd)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = 0;
    tmpbuf[1] = cmd;
    
    return HAL_I2C_Master_Transmit(g_pHI2COLED, OELD_I2C_ADDR, tmpbuf, 2, OLED_TIMEOUT);
}

/*
 *  函数名：OLED_WriteData
 *  功能描述：I2C发送命令给OLED
 *  输入参数：data-发送给OLED的写入GDRAM的数据
 *  输出参数：无
 *  返回值：0-成功, 其他值失败
 */
static int OLED_WriteData(uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = 0x40;
    tmpbuf[1] = data;
    
    return HAL_I2C_Master_Transmit(g_pHI2COLED, OELD_I2C_ADDR, tmpbuf, 2, OLED_TIMEOUT);
}

/*
 *  函数名：OLED_WriteNBytes
 *  功能描述：连续发送N个写入显存的数据
 *  输入参数：buf         - 数据buffer
 *            length - 数据个数
 *  输出参数：无
 *  返回值：0-成功, 其他值失败
 */
static int OLED_WriteNBytes(uint8_t *buf, uint16_t length)
{
    return HAL_I2C_Mem_Write(g_pHI2COLED, OELD_I2C_ADDR, 0x40, 1, buf, length, OLED_TIMEOUT);
}

/************** 1. 基础命令功能函数 **************/
/*
 *  函数名：OLED_SetContrastValue
 *  功能描述：设置OLED的对比度：发送命令0x81--->发送对比度值
 *  输入参数：value --> 对比度值，范围0~255
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetContrastValue(uint8_t value)
{
    OLED_WriteCmd(0x81);
    OLED_WriteCmd(value);
}

/************** 2. 滚动命令功能函数 **************/
/*
 *  函数名：OLED_H_Scroll
 *  功能描述：让指定页的像素朝着设置的方向按设置的频率水平滚动
 *  输入参数：dir--->滚动方向：朝左或者朝右
 *            start--->起始页
 *            fr_time--->滚动间隔时间，每隔这么多帧水平偏移一列滚动
 *            end--->结束页
 *  输出参数：无
 *  返回值：无
*/
static void OLED_H_Scroll(H_SCROLL_DIR dir, uint8_t start, uint8_t fr_time, uint8_t end)
{
    if((dir != H_RIGHT) && (dir != H_LEFT))     return;
    if((start>0x07) || (fr_time>0x07) || (end>0x07))    return;
    OLED_WriteCmd(dir);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(start);
    OLED_WriteCmd(fr_time);
    OLED_WriteCmd(end);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xFF);
}

/*
 *  函数名：OLED_HV_Scroll
 *  功能描述：让指定页的像素朝着设置的方向按设置的频率水平滚动且会将像素的行地址进行偏移offset这么多个单位
 *  输入参数：dir--->滚动方向：朝左或者朝右
 *            start--->起始页
 *            fr_time--->滚动间隔时间，每隔这么多帧水平偏移一列滚动
 *            end--->结束页
 *            offset--->行偏移单位
 *  输出参数：无
 *  返回值：无
*/
static void OLED_HV_Scroll(HV_SCROLL_DIR dir, uint8_t start, uint8_t fr_time, uint8_t end, uint8_t offset)
{
    if((dir != HV_RIGHT) && (dir != HV_LEFT))     return;
    if((start>0x07) || (fr_time>0x07) || (end>0x07) || (offset>0x3F))    return;
    OLED_WriteCmd(dir);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(start);
    OLED_WriteCmd(fr_time);
    OLED_WriteCmd(end);
    OLED_WriteCmd(offset);
}

/*
 *  函数名：OLED_SetVScrollArea
 *  功能描述：设置OLED像素垂直滚动的区域
 *  输入参数：area-->顶行区域
 *            row_num--->滚动的行区域
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetVScrollArea(uint8_t area, uint8_t row_num)
{
    if((area>0x3F) || (row_num>0x7F))       return;
    OLED_WriteCmd(0xA3);
    OLED_WriteCmd(area);
    OLED_WriteCmd(row_num);
}

/************** 3. 地址设置功能函数 **************/
static MEM_MODE mem_mode = PAGE_ADDR_MODE;  // 静态局部变量，保存OLED的地址模式的

/*
 *  函数名：OLED_SetColAddr_PAGE
 *  功能描述：设置OLED在页地址模式下的显示起始行地址
 *  输入参数：addr-->起始行地址
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetColAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 0x7F)   return;
    OLED_WriteCmd(0x00 + (addr & 0x0F));
    OLED_WriteCmd(0x10 + (addr>>4));
}

/*
 *  函数名：OLED_SetMemAddrMode
 *  功能描述：设置OLED的地址模式
 *  输入参数：mode-->地址模式：页地址模式、水平地址模式、垂直地址模式
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetMemAddrMode(MEM_MODE mode)
{
    if((mode != H_ADDR_MODE) && (mode != V_ADDR_MODE) && (mode != PAGE_ADDR_MODE))      return;
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(mode);
    mem_mode = mode;
}

/*
 *  函数名：OLED_SetColAddr_HV
 *  功能描述：设置OLED在水平地址模式或垂直地址模式下像素显示的起始行地址和结束行地址
 *  输入参数：start-->起始行地址
 *            end --->结束行地址
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetColAddr_HV(uint8_t start, uint8_t end)
{
    if(mem_mode == PAGE_ADDR_MODE)      return;
    if((start > 127) || (end > 127))    return;
    OLED_WriteCmd(0x21);
    OLED_WriteCmd(start);
    OLED_WriteCmd(end);
}

/*
 *  函数名：OLED_SetPageAddr_HV
 *  功能描述：设置OLED在水平地址模式或垂直地址模式下像素显示的起始页地址和结束页地址
 *  输入参数：start-->起始页地址
 *            end --->结束页地址
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetPageAddr_HV(uint8_t start, uint8_t end)
{
    if(mem_mode == PAGE_ADDR_MODE)      return;
    if((start > 7) || (end > 7))        return; 
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(start);
    OLED_WriteCmd(end);
}

/*
 *  函数名：OLED_SetPageAddr_PAGE
 *  功能描述：设置OLED在页地址模式下的显示起始页地址
 *  输入参数：addr-->起始页地址
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetPageAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 7)   return;
    OLED_WriteCmd(0xB0 + addr);
}

/************** 4. 硬件配置功能函数 **************/
/*
 *  函数名：OLED_SetDispStartLine
 *  功能描述：设置OLED从第line行开始显示，即将默认的0行偏移至line那一行
 *  输入参数：line-->显示的起始行
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetDispStartLine(uint8_t line)
{
    if(line > 63)       return;
    OLED_WriteCmd(0x40 + line);
}

/*
 *  函数名：OLED_SetMuxRatio
 *  功能描述：设置OLED复用率
 *  输入参数：ratio-->复用率值
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetMuxRatio(uint8_t ratio)
{
    if((ratio < 15) || (ratio > 63))      return;
    OLED_WriteCmd(0xA8);
    OLED_WriteCmd(ratio);
}

/*
 *  函数名：OLED_SetDispOffset
 *  功能描述：设置OLED的COM引脚偏移值
 *  输入参数：offset-->COM偏移值
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetDispOffset(uint8_t offset)
{
    if(offset > 63)     return;
    OLED_WriteCmd(0xD3);
    OLED_WriteCmd(offset);
}

/*
 *  函数名：OLED_SetComConfig
 *  功能描述：设置OLED的COM引脚属性
 *  输入参数：mode-->COM引脚模式是连续的还是可选择的
 *            state-->COM引脚翻转与否
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetComConfig(COM_PINS_MODE mode, COM_REMAP_STATE state)
{
    if((mode != COM_PIN_SEQ) && (mode != COM_PIN_ALT))      return;
    if((state != COM_NOREMAP) && (state != COM_REMAP))      return;
    
    OLED_WriteCmd(0xDA);
    OLED_WriteCmd(0x02 + (mode << 4) + (state << 5));
}

/************** 5. 时间设置功能函数 **************/
/*
 *  函数名：OLED_SetDCLK_Freq
 *  功能描述：设置OLED的扫描周期和晶振频率
 *  输入参数：div-->时钟分频系数
 *            freq-->晶振频率
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetDCLK_Freq(uint8_t div, uint8_t freq)
{
    if((div>0x0F) || (freq>0x0F))       return;
    OLED_WriteCmd(0xD5);
    OLED_WriteCmd(div + (freq<<4));
}

/*
 *  函数名：OLED_SetPreChargePeriod
 *  功能描述：设置OLED的预充电周期
 *  输入参数：phase1-->预充电阶段1时间
 *            phase2-->预充电阶段2时间
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetPreChargePeriod(uint8_t phase1, uint8_t phase2)
{
    if((phase1>0x0F) || (phase2>0x0F))       return;
    OLED_WriteCmd(0xD9);
    OLED_WriteCmd(phase1 + (phase2<<4));    
}

/*
 *  函数名：OLED_SetVcomhLevel
 *  功能描述：设置OLED的电压等级
 *  输入参数：level-->电压等级
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetVcomhLevel(VCOMH_LEVEL level)
{
    if((level != LEVEL_0) && (level != LEVEL_1) && (level != LEVEL_2))      return;
    OLED_WriteCmd(0xDB);
    OLED_WriteCmd(level);
}

/************** 6. 电荷碰撞功能函数 **************/
/*
 *  函数名：OLED_SetChargePump
 *  功能描述：打开或关闭OLED的电荷泵
 *  输入参数：state-->电荷泵打开或关闭
 *  输出参数：无
 *  返回值：无
*/
static void OLED_SetChargePump(CHARGE_PUMP_STATE state)
{
    if((state != PUMP_DISABLE) && (state != PUMP_ENABLE))   return;
    OLED_WriteCmd(0x8D);
    OLED_WriteCmd((state<<2) + 0x10);
}

/************** 7. 初始化函数 **************/
/*
 *  函数名：OLED_Init
 *  功能描述：初始化OLED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 */
void OLED_Init(void)
{   
    /*
     * 前提: 已经初始化的I2C通道
     * 本工程里已经: 
     *    使用MX_I2C1_Init初始化I2C通道
     *    使用HAL_I2C_MspInit初始化I2C引脚
     */
    
    OLED_SetMemAddrMode(PAGE_ADDR_MODE);    // 0. 设置地址模式
    OLED_SetMuxRatio(0x3F);                 // 1. 设置多路复用率
    OLED_SetDispOffset(0x00);               // 2. 设置显示的偏移值
    OLED_SetDispStartLine(0x00);            // 3. 设置起始行
    OLED_SEG_REMAP();                       // 4. 行翻转
    OLED_SCAN_REMAP();                      // 5. 正常扫描
    OLED_SetComConfig(COM_PIN_SEQ, COM_NOREMAP);          // 6. COM 引脚设置
    OLED_SetContrastValue(0x7F);            // 7. 设置对比度
    ENTIRE_DISP_OFF();                      // 8. 全屏点亮/熄灭
    DISP_NORMAL();                          // 9. 显示模式
    OLED_SetDCLK_Freq(0x00, 0x08);          // 10. 设置分频系数和频率增值
    OLED_SetChargePump(PUMP_ENABLE);        // 11. 使能电荷碰撞
    
    OLED_SetComConfig(COM_PIN_ALT, COM_NOREMAP);
    
    DISP_ON();
		
}

/************** 8. 基本驱动功能函数 **************/
/*
 *  函数名：OLED_SetPosition
 *  功能描述：设置像素显示的起始页和起始列地址
 *  输入参数：page-->页地址模式下的起始页地址
 *            col-->页地址模式下的起始行地址
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetPosition(uint8_t page, uint8_t col)
{
    OLED_SetPageAddr_PAGE(page);
    OLED_SetColAddr_PAGE(col);
}

/*
 *  函数名：OLED_Clear
 *  功能描述：清屏函数
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void OLED_Clear(void)
{
    uint8_t i = 0;
    uint8_t buf[128] = {0};
    
    for(i=0; i<8; i++)
    {
        OLED_SetPosition(i, 0);
        OLED_WriteNBytes(&buf[0], 128);
    }
}

/*
 *  函数名：OLED_PutChar
 *  功能描述：显示一个字符
 *  输入参数：x --> x坐标(0~15)
 *            y --> y坐标(0~7)
 *            c -->   显示的字符
 *  输出参数：无
 *  返回值：无
 */
void OLED_PutChar(uint8_t x, uint8_t y, char c)
{
    uint8_t page = y;
    uint8_t col  = x*8;
    
    if (y > 7 || x > 15)
        return;
    
    OLED_SetPosition(page, col);
    OLED_WriteNBytes((uint8_t*)&ascii_font[c][0], 8);
    
    OLED_SetPosition(page + 1, col);
    OLED_WriteNBytes((uint8_t*)&ascii_font[c][8], 8);
}

/*
 *  函数名：OLED_PrintString
 *  功能描述：显示一个字符串
 *  输入参数：x --> x坐标(0~15)
 *            y --> y坐标(0~7)
 *            str -->   显示的字符串
 *  输出参数：无
 *  返回值：打印了多少个字符
 */
int OLED_PrintString(uint8_t x, uint8_t y, const char *str)
{   
    int i = 0;
    while (str[i])
    {
        OLED_PutChar(x, y, str[i]);
        x++;
        if(x > 15)
        {
            x  = 0;
            y += 2;
        }
                
        i++;
    }
    return i;
}

/*
 *  函数名：OLED_ClearLine
 *  功能描述：清除一行
 *  输入参数：x - 从这里开始
 *            y - 清除这行
 *  输出参数：无
 *  返回值：无
 */
void OLED_ClearLine(uint8_t x, uint8_t y)
{
    for (;x < 16; x++)
    {
        OLED_PutChar(x, y, ' ');
    }
}

/*
 *  OLED_PrintHex
 *  功能描述：以16进制显示数值
 *  输入参数：x - x坐标(0~15)
 *            y - y坐标(0~7)
 *            val -   显示的数据
 *            pre -   非零时显示"0x"前缀
 *  输出参数：无
 *  返回值：打印了多少个字符
 */
int OLED_PrintHex(uint8_t x, uint8_t y, uint32_t val, uint8_t pre)
{
    uint8_t hex_tab[]={'0','1','2','3','4','5','6','7',\
                             '8','9','a','b','c','d','e','f'};
	int i, j;
	char arr[11];

	/* 提取数据 */
    arr[0] = '0';
    arr[1] = 'x';
    arr[10] = '\0';

    j = 2;
	for (i = 7; i >= 0; i--)
	{
        arr[j] = hex_tab[(val >> (i*4)) & 0xf];
        if ((j != 2) || (arr[j] != '0'))
        {
            j++;
        }        
	}
    if (j == 2)
        j++;
    arr[j] = '\0';

    if (pre)
    {
        OLED_PrintString(x, y, arr);
        return j;
    }
    else
    {
        OLED_PrintString(x, y, arr+2);
        return j - 2;
    }
}


/*
 *  OLED_PrintSignedVal
 *  功能描述：以10进制显示一个数值
 *  输入参数：x --> x坐标(0~15)
 *            y --> y坐标(0~7)
 *            val -->   显示的数据
 *  输出参数：无
 *  返回值：打印了多少个字符
*/
int OLED_PrintSignedVal(uint8_t x, uint8_t y, int32_t val)
{
    char str[16];
    char revert_str[16];
    int i = 0, j = 0;
    int mod;

    if (val < 0)
    {
        str[i++] = '-';
        val = 0 - val;
    }
    else if (val == 0)
    {
        str[i++] = '0';
    }

    while (val)
    {
        mod = val % 10;
        revert_str[j++] = mod + '0';
        val = val / 10;
    }

    while (j)
    {
        str[i++] = revert_str[j-1];
        j--;
    } 
    
    str[i] = '\0';
    OLED_PrintString(x, y, str);
    return i;
}

/**********************************************************************
 *  OLED_GetFrameBuffer
 *  功能描述：获得OLED的Framebuffer
 *  输入参数：无
 *  输出参数：pXres/pYres/pBpp - 用来保存分辨率、bpp
 *  返回值：Framebuffer首地址
 * -----------------------------------------------
 * 2023/08/31        V1.0     韦东山       创建
 ***********************************************************************/
void * OLED_GetFrameBuffer(uint32_t *pXres, uint32_t *pYres, uint32_t *pBpp)
{
    *pXres = FRAME_WIDTH;
    *pYres = FRAME_HEIGHT;
    *pBpp  = 1;
    return oledBuffer;
}

/**********************************************************************
 *  OLED_Flush
 *  功能描述：把Framebuffer的数据全部刷新到OLED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 * -----------------------------------------------
 * 2023/08/31        V1.0     韦东山       创建
 ***********************************************************************/
void OLED_Flush(void)
{
      uint8_t i;
      for(i = 0; i < 8; i++)
      {
         OLED_WriteCmd(0xb0+i); //设置行起始地址
         OLED_WriteCmd(0x00);   //设置低列起始地址
         OLED_WriteCmd(0x10);   //设置高列起始地址
         //for(n=0;n<128;n++)
         //  OLED_WriteData(oledBuffer[128*i+n]);
         OLED_WriteNBytes(&oledBuffer[128*i], 128);
    }
}

/**********************************************************************
 *  OLED_ClearFrameBuffer
 *  功能描述：把Framebuffer的数据全部清零
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 * -----------------------------------------------
 * 2023/08/31        V1.0     韦东山       创建
 ***********************************************************************/
void OLED_ClearFrameBuffer(void)
{
    memset(oledBuffer, 0, sizeof(oledBuffer));
}

void OLED_Backlight(uint8_t brightness)
{
	OLED_WriteCmd(0x81);
	OLED_WriteCmd(brightness);
}

// 反显示函数
void OLED_ColorTurn(uint8_t flag)
{
	if(flag == 0)
	{
		DISP_NORMAL();	// 正常显示
	}
	else if(flag == 1)
	{
		DISP_INVERSE();	// 反色显示
	}
		DISP_ON(); // 开启显示

}

// 屏幕旋转180度
void OLED_DisplayTurn(uint8_t flag)
{
	if(flag == 0)
	{
		OLED_WriteCmd(0xA1);
		OLED_WriteCmd(0xC8);
	}
	else if(flag == 1)
	{
		OLED_WriteCmd(0xA0);
		OLED_WriteCmd(0xC0);
	}
}


void OLED_DisplayOn(void)
{
	DISP_ON();
}

void OLED_DisplayOff(void)
{
	DISP_ON();
}



/**********************************************************************
 * 函数名称： OLED_Test
 * 功能描述： OLED测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/03        V1.0     韦东山       创建
 ***********************************************************************/
void OLED_Test(void)
{
    OLED_Init();
	// 清屏
	OLED_Clear();
    
	while (1)
	{
		// 在(0, 0)打印'A'
		OLED_PutChar(0, 0, 'A');
		// 在(1, 0)打印'Y'
		OLED_PutChar(1, 0, 'Y');
		// 在第0列第2页打印一个字符串"Hello World!"
		OLED_PrintString(0, 2, "Hello World!");
	}
}


