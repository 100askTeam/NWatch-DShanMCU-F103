/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2014 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef LANG_H_
#define LANG_H_

#if LANGUAGE == 0
#include "english.h"
#elif LANGUAGE == 1
#include "german.h"
#elif LANGUAGE == 2
#include "russian.h"
#else
#error "Unknown language set"
#endif

#endif /* LANG_H_ */
