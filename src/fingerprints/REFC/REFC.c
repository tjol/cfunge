/*
 * cfunge - a conformant Befunge93/98/08 interpreter in C.
 * Copyright (C) 2008 Arvid Norlander <anmaster AT tele2 DOT se>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at the proxy's option) any later version. Arvid Norlander is a
 * proxy who can decide which future versions of the GNU General Public
 * License can be used.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "REFC.h"
#include "../../stack.h"
#include "../../vector.h"

#include <stdio.h>
#include <assert.h>

#define ALLOCCHUNK 5
static fungePosition *references = NULL;
static size_t referencesTop = 0;
static size_t referencesSize = 0;

static void FingerREFCReference(instructionPointer * ip) {
	FUNGEDATATYPE x, y;
	y = StackPop(ip->stack);
	x = StackPop(ip->stack);
	if (referencesSize == referencesTop) {
		references = cf_realloc(references, (referencesSize + ALLOCCHUNK) * sizeof(fungePosition));
		// FIXME: Broken state if realloc fails
		if (references == NULL) {
			ipReverse(ip);
			return;
		}
		referencesSize += ALLOCCHUNK;
	}

	references[referencesTop].x = x;
	references[referencesTop].y = y;
	StackPush(referencesTop, ip->stack);
	referencesTop++;
}

static void FingerREFCDereference(instructionPointer * ip) {
	FUNGEDATATYPE ref;
	ref = StackPop(ip->stack);
	if ((ref < 0) || ((size_t)ref > referencesTop)) {
		ipReverse(ip);
		return;
	}
	StackPushVector(&references[ref], ip->stack);
}

static inline bool InitReferences(void) {
	assert(!references);
	references = cf_malloc(ALLOCCHUNK * sizeof(fungePosition));
	if (!references)
		return false;
	referencesSize = ALLOCCHUNK;
	return true;
}


bool FingerREFCload(instructionPointer * ip) {
	if (!references)
		if (!InitReferences())
			return false;

	if (!OpcodeStackAdd(ip, 'D', &FingerREFCDereference))
		return false;
	if (!OpcodeStackAdd(ip, 'R', &FingerREFCReference))
		return false;
	return true;
}
