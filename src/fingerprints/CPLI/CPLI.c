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

#include "CPLI.h"
#include "../../stack.h"

#include <math.h>

#ifndef HAVE_sqrtl
#  define sqrtl sqrt
#endif

/// A - add
static void FingerCPLIadd(instructionPointer * ip)
{
	fungeCell ar, ai, br, bi;
	bi = StackPop(ip->stack);
	br = StackPop(ip->stack);
	ai = StackPop(ip->stack);
	ar = StackPop(ip->stack);
	StackPush(ip->stack, ar + br);
	StackPush(ip->stack, ai + bi);
}

/// D - div
static void FingerCPLIdiv(instructionPointer * ip)
{
	fungeCell ar, ai, br, bi, denom;
	bi = StackPop(ip->stack);
	br = StackPop(ip->stack);
	ai = StackPop(ip->stack);
	ar = StackPop(ip->stack);
	denom = bi * bi + br * br;
	if (denom != 0) {
		StackPush(ip->stack, (ai*bi + ar*br) / denom);
		StackPush(ip->stack, (ai*br - ar*bi) / denom);
	} else {
		StackPush(ip->stack, 0);
		StackPush(ip->stack, 0);
	}
}

/// M - mul
static void FingerCPLImul(instructionPointer * ip)
{
	fungeCell ar, ai, br, bi;
	bi = StackPop(ip->stack);
	br = StackPop(ip->stack);
	ai = StackPop(ip->stack);
	ar = StackPop(ip->stack);
	StackPush(ip->stack, ar*br - ai*bi);
	StackPush(ip->stack, ar*bi + ai*br);
}

/// O - out
static void FingerCPLIout(instructionPointer * ip)
{
	fungeCell r, i;
	i = StackPop(ip->stack);
	r = StackPop(ip->stack);
	printf("%" FUNGECELLPRI, r);
	if (i > 0)
		cf_putchar_maybe_locked('+');
	printf("%" FUNGECELLPRI "i ", i);
}

/// S - sub
static void FingerCPLIsub(instructionPointer * ip)
{
	fungeCell ar, ai, br, bi;
	bi = StackPop(ip->stack);
	br = StackPop(ip->stack);
	ai = StackPop(ip->stack);
	ar = StackPop(ip->stack);
	StackPush(ip->stack, ar - br);
	StackPush(ip->stack, ai - bi);
}

/// V - abs
static void FingerCPLIabs(instructionPointer * ip)
{
	fungeCell r, i;
	long double tmp;
	i = StackPop(ip->stack);
	r = StackPop(ip->stack);
	tmp = sqrtl((long double)(r * r + i * i));
	StackPush(ip->stack, (fungeCell)tmp);
}

bool FingerCPLIload(instructionPointer * ip)
{
	ManagerAddOpcode(CPLI, 'A', add)
	ManagerAddOpcode(CPLI, 'D', div)
	ManagerAddOpcode(CPLI, 'M', mul)
	ManagerAddOpcode(CPLI, 'O', out)
	ManagerAddOpcode(CPLI, 'S', sub)
	ManagerAddOpcode(CPLI, 'V', abs)
	return true;
}
