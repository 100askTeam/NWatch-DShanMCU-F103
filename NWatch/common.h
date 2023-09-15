/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "config.h"
#include "util.h"
#include "typedefs.h"
#include "debug.h"


#include "buttons.h"
#include "millis.h"
#include "functions.h"
#include "alarms.h"
#include "diag.h"
#include "m_display.h"
#include "games.h"
#include "timedate.h"
#include "settings.h"
#include "sleep.h"
#include "sound.h"
#include "m_main.h"
#include "game1.h"
#include "game2.h"
#include "game3.h"
#include "stopwatch.h"
#include "torch.h"
//#include "screenshot.h"    //输出oledbuff里面的数据
#include "normal.h"
//#include "lowbatt.h"
#include "ui.h"
#include "tunemaker.h"

//#include "system.h"
#include "global.h"
#include "display.h"
#include "time.h"
#include "alarm.h"
#include "pwrmgr.h"
#include "appconfig.h"
//#include "disco.h"
#include "tune.h"
#include "animation.h"
#include "draw.h"
#include "menu.h"

#include "english.h"
#include "lang.h"
#include "tunes.h"
//#include "discos.h"
#include "resources.h"

#include "watchface.h"

#include "reset.h" 											// 100SAK
#include "driver_systick.h"							// 100SAK
#include "driver_lcd.h"									// 100SAK
#include "driver_passive_buzzer.h"			// 100SAK
#include "driver_ir_receiver.h"					// 100SAK
#include "driver_color_led.h"						// 100SAK
#include "driver_dht11.h"								// 100SAK
#include "driver_spiflash_w25q64.h"			// 100SAK

#include "gui_log_console.h"

#endif /* COMMON_H_ */
