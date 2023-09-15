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


#include "common.h"

#define OPTION_COUNT	1

static bool reset_flag = false;

static prev_menu_s prevMenuData;

static void mSelect(void);
static void itemLoader(byte);
static display_t mDraw(void);
static void setReset(void);

void mResetOpen(void)
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_RESETMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	menuData.func.draw = mDraw;

	setPrevMenuOpen(&prevMenuData, mResetOpen);

	beginAnimation2(NULL);
}

static void mSelect(void)
{
	bool isExiting = exitSelected();
	if(isExiting && reset_flag)
	{
		appconfig_reset();
		HAL_NVIC_SystemReset(); // Restart
	}
	
	setPrevMenuExit(&prevMenuData);
	doAction(isExiting);
}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOption_P(0, PSTR(STR_RESET), menu_rotate, setReset);

	addBackOption();
}

static display_t mDraw(void)
{
	if(menuData.selected == 0)
	{
		char buff[2];		
		sprintf_P(buff, reset_flag ? PSTR("Y") : PSTR("N"));
		
		draw_string(buff, NOINVERT, 61, 30);
	}
	
	return DISPLAY_DONE;
}

static void setReset(void)
{
	reset_flag = !reset_flag;
}

