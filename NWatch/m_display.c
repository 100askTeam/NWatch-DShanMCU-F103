/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#include "driver_lcd.h"

#define OPTION_COUNT	5

static prev_menu_s prevMenuData;

static void mSelect(void);
static void itemLoader(byte);
static void setBrightness(void);
static void setInvert(void);
static void setRotate(void);
#if COMPILE_ANIMATIONS
static void setAnimations(void);
#endif
static void setMenuOptions(void);

void mDisplayOpen()
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_DISPLAYMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);

//	setMenuOptions();

	setPrevMenuOpen(&prevMenuData, mDisplayOpen);

	beginAnimation2(NULL);
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
		appconfig_save();
	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());
}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOptions();
	addBackOption();
}

static void setBrightness()
{
 	byte brightness = appConfig.brightness;
	if(brightness > 4)
		brightness = 0;
	
	brightness++;
	appConfig.brightness = brightness;
	
	byte _brightness = 0;

	switch(brightness)           //…Ë÷√ª≠± —’…´
	{
		case 0:	_brightness = 0x01; break;
		case 1:	_brightness = 0x0f; break;
		case 2:	_brightness = 0x5f; break;
		case 3:	_brightness = 0xff; break;
	}	

	LCD_SetDisplayBacklight(_brightness);
}

static void setInvert()
{
	appConfig.invert = !appConfig.invert;
	
	LCD_SetColorTurn(appConfig.invert);
}

static void setRotate() 
{
	if(appConfig.display180 == false)
		appConfig.display180 = true;
	else if(appConfig.display180 == true)
		appConfig.display180 = false;
	else
		appConfig.display180 = false;

	LCD_SetDisplayTurn(appConfig.display180);
}

static void setLEDs() {
  appConfig.CTRL_LEDs = !appConfig.CTRL_LEDs;
	ColorLED_Set(0x00000000);
}

#if COMPILE_ANIMATIONS
static void setAnimations()
{
	appConfig.animations = !appConfig.animations;
}
#endif

static void setMenuOptions()
{
	setMenuOption_P(0, PSTR(STR_BRIGHTNESS), menu_brightness[appConfig.brightness], setBrightness);
	setMenuOption_P(1, PSTR(STR_INVERT), menu_invert, setInvert);
	setMenuOption_P(2, PSTR(STR_ROTATE), menu_rotate, setRotate);
#if COMPILE_ANIMATIONS
	setMenuOption_P(3, PSTR(STR_ANIMATIONS), menu_anim[appConfig.animations], setAnimations);
#endif
	setMenuOption_P(4, PSTR(STR_LEDS), menu_LEDs[appConfig.CTRL_LEDs], setLEDs);
	

}
