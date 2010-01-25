/* -*- mode: C; coding: utf-8; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * cfunge - A standard-conforming Befunge93/98/109 interpreter in C.
 * Copyright (C) 2008-2010 Arvid Norlander <anmaster AT tele2 DOT se>
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

/**
 * @file
 * execute.c contains the implementation of the = instruction.
 */

#ifndef FUNGE_HAD_SRC_INSTRUCTIONS_EXECUTE_H
#define FUNGE_HAD_SRC_INSTRUCTIONS_EXECUTE_H

#include "../global.h"
#include "../stack.h"
#include "../ip.h"
#include "../stack.h"

/// This is the = instruction.
FUNGE_ATTR_FAST FUNGE_ATTR_NONNULL
void run_system_execute(instructionPointer * restrict ip);

#endif
