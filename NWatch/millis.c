/*
 * Project: Lightweight millisecond tracking library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/millisecond-tracking-library-for-avr/
 */

#ifndef ARDUINO


#include "common.h"
#include "driver_systick.h"

//#include "led.h"
//#include "misc.h"

//millis_t milliseconds;

// Get current milliseconds
millis_t millis_get(void)
{
	return SysTickGetMs();
}

#endif
