/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2014 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef TUNE_H_
#define TUNE_H_
#include "typedefs.h"
///#include "beep.h"
#define TUNEMEM_RAM 0
#define TUNEMEM_EEPROM 1
#define TUNEMEM_PROGMEM 2


// Where to store tunes
#define TUNEMEM_TYPE	TUNEMEM_RAM

#if TUNEMEM_TYPE == TUNEMEM_PROGMEM
#define TUNEMEM PROGMEM
#elif TUNEMEM_TYPE == TUNEMEM_EEPROM
#define TUNEMEM EEMEM
#else
#define TUNEMEM 
#endif


typedef enum
{
	LA =262,
	LB =294,
	LC =330,
	LD =349,
	LE =392,
	LF =440,
	LG =494,
	
	MA =523,
	MB =578,
	MC =659,
	MD =698,
	ME =784,
	MF =880,
	MG =988,
	
	HA =1064,
	HB =1175,
	HC =1318,
	HD =1397,
	HE =1568,
	HF =1760,
	HG =1976
}music_t;

// TONE_VALUE = F_CPU / TONE_FREQ / 16
// TONE_VALUE should be kept below 256 (1960Hz) otherwise the datatype will be bumped up from char to int
typedef enum
{
	TONE_STOP = 0,
	TONE_PAUSE = 1,
	TONE_REPEAT = 2,
	TONE_2KHZ = 250,
	TONE_2_5KHZ = 200,
	TONE_3KHZ = 166,
	TONE_3_5KHZ = 143,
	TONE_4KHZ = 125,
	TONE_4_5KHZ = 111,
	TONE_5KHZ = 100,
} tone_t;

typedef enum
{
	VOL_OTHER = 0,
	VOL_UI = 1,
	VOL_ALARM = 2,
	VOL_HOUR = 3
} vol_t;

typedef enum
{
	PRIO_MIN = 0,
	PRIO_LOW = 1,
	PRIO_NML = 2,
	PRIO_HIGH = 3,
	PRIO_MAX = 255
} tonePrio_t;

#define PRIO_UI		PRIO_LOW
#define PRIO_ALARM	PRIO_HIGH
#define PRIO_HOUR	PRIO_NML

typedef void (*buzzFinish_f)(void);

#define buzzer_stop()	(buzzer_buzz(0, TONE_STOP, PRIO_MAX, NULL))

typedef uint32_t tune_t;

void tune_play(const tune_t*, vol_t, tonePrio_t);
void tune_stop(tonePrio_t);

#endif /* TUNE_H_ */
