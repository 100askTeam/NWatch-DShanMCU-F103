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

