/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2014 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

// Remove const when TUNEMEM is EEPMEM?



const tune_t STAY[]={
	//5353
	ME<<4 | 250,
	MC<<4 | 250,
	ME<<4 | 250,
	MC<<4 | 250,
	//222321
	MB<<4 | 125,
	MB<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	MB<<4 | 125,
	MA<<4 | 250,
	//7115
	LG<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	ME<<4 | 500,
	//177777111
	MA<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 250,
	//1715
	MA<<4 | 125,
	LG<<4 | 125,
	MA<<4 | 125,
	ME<<4 | 500,
	//177777111
	MA<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 250,
	//1715
	MA<<4 | 125,
	LG<<4 | 125,
	MA<<4 | 125,
	ME<<4 | 500,
	//177777111
	MA<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	LG<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 250,
	//1715
	MA<<4 | 125,
	LG<<4 | 125,
	MA<<4 | 125,
	ME<<4 | 500,
	//71275
	LG<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MG<<4 | 125,
	ME<<4 | 500,
	
	TONE_REPEAT

};  //ÐýÂÉ


const tune_t TUNE[]={
	LF<<4 | 250,
	LC<<4 | 250,
	HF<<4 | 250,
	MC<<4 | 250,
	
	LD<<4 | 250,
	MA<<4 | 250,
	MD<<4 | 250,
	MA<<4 | 250,
	
	LE<<4 | 250,
	MB<<4 | 250,
	ME<<4 | 250,
	MB<<4 | 250,
	
	MA<<4 | 250,
	ME<<4 | 250,
	HA<<4 | 250,
	ME<<4 | 250,
	
	LF<<4 | 250,
	LC<<4 | 250,
	HF<<4 | 250,
	MC<<4 | 250,
	
	LD<<4 | 250,
	MA<<4 | 250,
	MD<<4 | 250,
	MA<<4 | 250,
	
	LE<<4 | 250,
	MB<<4 | 250,
	ME<<4 | 250,
	MB<<4 | 250,
	//1 5123
	MA<<4 | 500,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	//2111
	MB<<4 | 250,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 500,
	
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	
	//212233
	MB<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 250,
	MC<<4 | 125,
	
	//35123
	MC<<4 | 500,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	
	//2111
	MB<<4 | 250,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 500,
	
	
	//05123
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	//212253
	MB<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	MB<<4 | 125,
	ME<<4 | 250,
	MC<<4 | 125,
	
	//334
	MC<<4 | 500,
	MC<<4 | 250,
	MD<<4 | 250,
	
	//55555
	ME<<4 | 250,
	ME<<4 | 125,
	ME<<4 | 125,
	ME<<4 | 250,
	ME<<4 | 250,
	
	//531134
	ME<<4 | 250,
	MC<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	MC<<4 | 250,
	MD<<4 | 125,
	
	//55555
	ME<<4 | 250,
	ME<<4 | 125,
	ME<<4 | 125,
	ME<<4 | 250,
	ME<<4 | 250,
	//531 112
	ME<<4 | 250,
	MC<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	
	//33333
	MC<<4 | 250,
	MC<<4 | 125,
	MC<<4 | 125,
	MC<<4 | 250,
	MC<<4 | 250,
	//366321
	MC<<4 | 250,
	LF<<4 | 250,
	MF<<4 | 125,
	MC<<4 | 125,
	MB<<4 | 125,
	LA<<4 | 250,
	
	//20
	MB<<4 | 500,
	TONE_PAUSE<<8 | 125,
	
	//05123
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	//2111
	MB<<4 | 250,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 500,
	//05123
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	
	//212233
	MB<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 250,
	MC<<4 | 125,
	
	//35123
	MC<<4 | 500,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	//2111
	MB<<4 | 250,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 500,
	//05123
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	//212253
	MB<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	MB<<4 | 125,
	ME<<4 | 250,
	MC<<4 | 125,
	
	//334
	MC<<4 | 500,
	MC<<4 | 250,
	MD<<4 | 250,
	
	//55555
	ME<<4 | 250,
	ME<<4 | 125,
	ME<<4 | 125,
	ME<<4 | 250,
	ME<<4 | 250,
	
	//531134
	ME<<4 | 250,
	MC<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	MC<<4 | 250,
	MD<<4 | 125,
	
	//55555
	ME<<4 | 250,
	ME<<4 | 125,
	ME<<4 | 125,
	ME<<4 | 250,
	ME<<4 | 250,
	//531 112
	ME<<4 | 250,
	MC<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	
	//33333
	MC<<4 | 250,
	MC<<4 | 125,
	MC<<4 | 125,
	MC<<4 | 250,
	MC<<4 | 250,
	//363216
	MC<<4 | 500,
	MF<<4 | 125,
	MC<<4 | 125,
	MB<<4 | 125,
	MA<<4 | 125,
	LF<<4 | 125,
	
	//10
	MA<<4 | 500,
	TONE_PAUSE<<8 | 250,
	//0
	//TONE_PAUSE<<8 | 3000,
	
	//55555
	ME<<4 | 250,
	ME<<4 | 125,
	ME<<4 | 125,
	ME<<4 | 250,
	ME<<4 | 250,
	
	//531134
	ME<<4 | 250,
	MC<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 125,
	MC<<4 | 250,
	MD<<4 | 125,
	
	//55555
	ME<<4 | 250,
	ME<<4 | 125,
	ME<<4 | 125,
	ME<<4 | 250,
	ME<<4 | 250,
	//531 112
	ME<<4 | 250,
	MC<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	
	//33333
	MC<<4 | 250,
	MC<<4 | 125,
	MC<<4 | 125,
	MC<<4 | 250,
	MC<<4 | 250,
	//366321
	MC<<4 | 250,
	LF<<4 | 250,
	MF<<4 | 125,
	MC<<4 | 125,
	MB<<4 | 125,
	LA<<4 | 250,
	
	//20
	MB<<4 | 500,
	TONE_PAUSE<<8 | 125,
	
	//05123
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	//2111
	MB<<4 | 250,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 500,
	//05123
	TONE_PAUSE<<8 | 125,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	
	//212233
	MB<<4 | 250,
	MA<<4 | 125,
	MB<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 250,
	MC<<4 | 125,
	
	//35123
	MC<<4 | 500,
	LE<<4 | 125,
	MA<<4 | 125,
	MB<<4 | 125,
	MC<<4 | 125,
	
	//2111
	MB<<4 | 250,
	MA<<4 | 125,
	MA<<4 | 125,
	MA<<4 | 500,
	
	TONE_PAUSE<<8 | 1000,
	
	TONE_REPEAT

};  //ÐýÂÉ



const tune_t tuneUsbPlugin[] TUNEMEM = {
	TONE_2KHZ<<8 | 50,
	TONE_PAUSE<<8 | 100,
	TONE_2_5KHZ<<8 | 50,
	TONE_PAUSE<<8 | 100,
	TONE_3KHZ<<8 | 200,
	TONE_STOP
};

const tune_t tuneUsbUnplug[] TUNEMEM = {
	TONE_3KHZ<<8 | 50,
	TONE_PAUSE<<8 | 100,
	TONE_2_5KHZ<<8 | 50,
	TONE_PAUSE<<8 | 100,
	TONE_2KHZ<<8 | 200,
	TONE_STOP
};

const tune_t tuneUsbCharged[] TUNEMEM = {
	TONE_2KHZ<<8 | 150,
	TONE_PAUSE<<8 | 150,
	TONE_2KHZ<<8 | 150,
	TONE_PAUSE<<8 | 150,
	TONE_4KHZ<<8 | 250,
	TONE_4KHZ<<8 | 250,
	TONE_STOP
};

const tune_t tuneHour[] TUNEMEM = {
	TONE_2_5KHZ<<8 | 150,
	TONE_PAUSE<<8 | 80,
	TONE_2_5KHZ<<8 | 150,
	TONE_STOP
};

const tune_t tuneAlarm[] TUNEMEM = {
	TONE_2KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_2KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_3KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_3KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_REPEAT
};

const tune_t tuneBtn1[] TUNEMEM = {
	TONE_2KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_3KHZ<<8 | 100,
	TONE_STOP
};

const tune_t tuneBtn2[] TUNEMEM = {
	TONE_3KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_3KHZ<<8 | 100,
	TONE_STOP
};

const tune_t tuneBtn3[] TUNEMEM = {
	TONE_3KHZ<<8 | 100,
	TONE_PAUSE<<8 | 50,
	TONE_2KHZ<<8 | 100,
	TONE_STOP
};
