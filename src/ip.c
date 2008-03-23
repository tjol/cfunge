/* -*- mode: C; coding: utf-8; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
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

#include "global.h"
#include "ip.h"
#include "vector.h"
#include "interpreter.h"
#include "settings.h"
#include "fingerprints/manager.h"
#include "stack.h"
#include "funge-space/funge-space.h"

#include <assert.h>

static inline bool ipCreateInPlace(instructionPointer *me) __attribute__((nonnull,warn_unused_result));

static inline bool ipCreateInPlace(instructionPointer *me)
{
	assert(me);
	me->position.x         = 0;
	me->position.y         = 0;
	me->delta.x            = 1;
	me->delta.y            = 0;
	me->storageOffset.x    = 0;
	me->storageOffset.y    = 0;
	me->mode               = ipmCODE;
	me->NeedMove           = true;
	me->StringLastWasSpace = false;
	me->stackstack         = StackStackCreate();
	if (!me->stackstack)
		return false;
	me->stack              = me->stackstack->stacks[me->stackstack->current];
	me->ID                 = 0;
	if (SettingEnableFingerprints) {
		if (!ManagerCreate(me))
			return false;
	}
	return true;
}

instructionPointer * ipCreate(void)
{
	instructionPointer * tmp = cf_malloc(sizeof(instructionPointer));
	if (!ipCreateInPlace(tmp))
		return NULL;
	return tmp;
}

#ifdef CONCURRENT_FUNGE
static inline bool ipDuplicateInPlace(const instructionPointer * old, instructionPointer * new) {
	assert(old);
	assert(new);
	new->position.x         = old->position.x;
	new->position.y         = old->position.y;
	new->delta.x            = old->delta.x;
	new->delta.y            = old->delta.y;
	new->storageOffset.x    = old->storageOffset.x;
	new->storageOffset.y    = old->storageOffset.y;
	new->mode               = ipmCODE;
	new->NeedMove           = old->NeedMove;
	new->StringLastWasSpace = old->StringLastWasSpace;
	new->stackstack         = StackStackDuplicate(old->stackstack);
	if (!new->stackstack)
		return false;

	new->stack              = new->stackstack->stacks[new->stackstack->current];
	if (SettingEnableFingerprints) {
		if (!ManagerDuplicate(old, new))
			return false;
	}
	return true;
}
#endif

static inline void ipFreeResources(instructionPointer * ip)
{
	if (!ip)
		return;
	if (ip->stackstack) {
		StackStackFree(ip->stackstack);
		ip->stackstack = NULL;
	}
	ip->stack = NULL;
	if (SettingEnableFingerprints) {
		ManagerFree(ip);
	}
}


void ipFree(instructionPointer * restrict ip)
{
	if (!ip)
		return;
	ipFreeResources(ip);
	cf_free(ip);
}

void ipForward(int_fast64_t steps, instructionPointer * restrict ip)
{
	assert(ip != NULL);
	ip->position.x += ip->delta.x * steps;
	ip->position.y += ip->delta.y * steps;
	fungeSpaceWrap(&ip->position, &ip->delta);
}

void ipTurnRight(instructionPointer * restrict ip)
{
	FUNGEVECTORTYPE tmpX;

	assert(ip != NULL);

	tmpX        = ip->delta.x;
	ip->delta.x = -ip->delta.y;
	ip->delta.y = tmpX;
}

void ipTurnLeft(instructionPointer * restrict ip)
{
	FUNGEVECTORTYPE tmpX;

	assert(ip != NULL);

	tmpX        = ip->delta.x;
	ip->delta.x = ip->delta.y;
	ip->delta.y = -tmpX;
}

void ipSetDelta(instructionPointer * restrict ip, const ipDelta * restrict delta)
{
	assert(ip != NULL);
	assert(delta != NULL);
	ip->delta.x = delta->x;
	ip->delta.y = delta->y;
}

void ipSetPosition(instructionPointer * restrict ip, const fungePosition * restrict position)
{
	assert(ip != NULL);
	assert(position != NULL);
	ip->position.x = position->x;
	ip->position.y = position->y;
	fungeSpaceWrap(&ip->position, &ip->delta);
}


/***********
 * IP list *
 ***********/

#ifdef CONCURRENT_FUNGE
ipList* ipListCreate(void)
{
	ipList * tmp = cf_malloc(sizeof(ipList) + sizeof(instructionPointer));
	if (!tmp)
		return NULL;
	if (!ipCreateInPlace(&tmp->ips[0]))
		return NULL;
	tmp->size = 1;
	tmp->top = 0;
	tmp->highestID = 0;
	return tmp;
}

void ipListFree(ipList* me)
{
	if (!me)
		return;
	for (size_t i = 0; i <= me->top; i++) {
		ipFreeResources(&me->ips[i]);
	}
	cf_free(me);
}

ssize_t ipListDuplicateIP(ipList** me, size_t index)
{
	ipList *list;

	assert(me != NULL);
	assert(*me != NULL);
	assert(index <= (*me)->top);

	// Grow
	list = cf_realloc(*me, sizeof(ipList) + ((*me)->size + 1) * sizeof(instructionPointer));
	if (!list)
		return -1;
	*me = list;
	list->size++;
/*
	Splitting examples.

	Thread index 3 splits (to 3a)
	0  | 1  | 2  | 3  | 4   | 5  | 6
	---------------------------------
	t0 | t1 | t3 | t3 | t4  | t5 |
	t0 | t1 | t2 | t3 | t3a | t4 | t5

*/
	// Do we need to move any upwards?
	if (index != list->top) {
		/* Move upwards:
		 * t0 | t1 | t2 |
		 * t10|    | t1 | t2
		 */
		for (size_t i = list->top; i > index; i--) {
			list->ips[i] = list->ips[i - 1];
		}
	}
	/* Duplicate:
	 * t0 | t1  | t2 |
	 * t0 | t0a | t1 | t2
	 */
	ipDuplicateInPlace(&list->ips[index], &list->ips[index + 1]);

	// Here we mirror new IP and do ID changes.
	index++;
	ipReverse(&list->ips[index]);
	ipForward(1, &list->ips[index]);
	list->ips[index].ID = ++list->highestID;
	list->top++;
	return index - 1;
}

ssize_t ipListTerminateIP(ipList** me, size_t index)
{
	ipList *list;

	assert(me != NULL);
	assert(*me != NULL);

	list = *me;

/*
	Terminate examples.

	Thread index 3 dies
	0  | 1  | 2  | 3  | 4  | 5
	---------------------------
	t0 | t1 | t2 | t3 | t4 | t5
	t0 | t1 | t3 | t4 | t5 |

*/
	ipFreeResources(&list->ips[index]);
	// Do we need to move downwards?
	if (index != list->top) {
		/* Move downwards:
		 * t0 |    | t2 | t3
		 * t0 | t2 | t3 |
		 */
		for (size_t i = index; i < list->top; i++) {
			list->ips[i] = list->ips[i + 1];
		}
	}
	// Shrink
	list = cf_realloc(list, sizeof(ipList) + (list->size - 1) * sizeof(instructionPointer));
	if (!list)
		return -1;
	*me = list;
	list->top--;
	list->size--;
	return (index > 0) ? index - 1 : 0;
}

#endif
