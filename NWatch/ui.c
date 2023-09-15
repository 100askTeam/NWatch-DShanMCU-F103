/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"


void drawBattery()
{
	///uint battLevel =  battery_voltage();  
		uint battLevel = 3000;
		const byte* battIcon;
		if(battLevel < 3100)
			battIcon = battIconEmpty;
		else if(battLevel < 3200)
			battIcon = battIconLow;
		else if(battLevel < 3300)
			battIcon = battIconHigh;
		else
			battIcon = battIconFull;

	draw_bitmap(0, FRAME_HEIGHT - 8, battIcon, 16, 8, NOINVERT, 0);
}
