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

#ifndef _HAD_SRC_INSTRUCTIONS_ITERATE_H
#define _HAD_SRC_INSTRUCTIONS_ITERATE_H

#include "../global.h"
#include "../stack.h"
#include "../ip.h"
#include "../stack.h"
#include "../funge-space/funge-space.h"


#ifdef CONCURRENT_FUNGE
void RunIterate(instructionPointer * restrict ip, ipList ** IPList, ssize_t * restrict threadindex) __attribute__((nonnull,FUNGE_IN_FAST));
#else
void RunIterate(instructionPointer * restrict ip) __attribute__((nonnull,FUNGE_IN_FAST));
#endif

#endif
