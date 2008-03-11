/*
 * cfunge08 - a conformant Befunge93/98/08 interpreter in C.
 * Copyright (C) 2008 Arvid Norlander <anmaster AT tele2 DOT se>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"
#include "vector.h"
#include "stack.h"
#include "ip.h"

// How many new items to allocate in one go?
#define ALLOCCHUNKSIZE 10

fungeStack *
StackCreate(void)
{
	fungeStack * tmp = cf_malloc(sizeof(fungeStack));
	if (tmp == NULL)
		return NULL;
	tmp->entries = cf_malloc(ALLOCCHUNKSIZE * sizeof(FUNGEDATATYPE));
	if (tmp->entries == NULL)
		return NULL;
	tmp->size = ALLOCCHUNKSIZE;
	tmp->top = 0;
	return tmp;
}

void
StackFree(fungeStack * stack)
{
	if (!stack || !stack->entries)
		return;
	cf_free(stack->entries);
	stack->entries = NULL;
	cf_free(stack);
}


void
StackPush(FUNGEDATATYPE value, fungeStack * stack)
{
	// Do we need to realloc?
	if (stack->top == stack->size) {
		stack->entries = cf_realloc(stack->entries, (stack->size + ALLOCCHUNKSIZE) * sizeof(FUNGEDATATYPE));
		stack->entries[stack->top] = value;
		stack->top++;
		stack->size += ALLOCCHUNKSIZE;
	} else {
		stack->entries[stack->top] = value;
		stack->top++;
	}
}

void
StackPushVector(const fungeVector * value, fungeStack * stack) {
	// TODO: Optimize
	StackPush(value->x, stack);
	StackPush(value->y, stack);
}


FUNGEDATATYPE
StackPop(fungeStack * stack)
{
	if (stack->top == 0) {
		return 0;
	} else {
		FUNGEDATATYPE tmp = stack->entries[stack->top - 1];
		stack->top--;
		return tmp;
	}
}

void
StackPopDiscard(fungeStack * stack)
{
	if (stack->top == 0) {
		return;
	} else {
		stack->top--;
	}
}

fungeVector
StackPopVector (fungeStack * stack) {
	// TODO Optimize
	FUNGEVECTORTYPE x, y;
	y = StackPop(stack);
	x = StackPop(stack);
	return (fungeVector) { .x = x, .y = y };
}

FUNGEDATATYPE
StackPeek(fungeStack * stack)
{
	if (stack->top == 0) {
		return 0;
	} else {
		return stack->entries[stack->top - 1];
	}
}


void
StackClear(fungeStack * stack)
{
	stack->top = 0;
}


void
StackDupTop(fungeStack * stack)
{
	// TODO: Optimize instead of doing it this way
	FUNGEDATATYPE tmp;

	tmp = StackPeek(stack);
	StackPush(tmp, stack);
}


void
StackSwapTop(fungeStack * stack)
{
	// TODO: Optimize instead of doing it this way
	FUNGEDATATYPE a, b;
	a = StackPop(stack);
	b = StackPop(stack);
	StackPush(a, stack);
	StackPush(b, stack);
}


fungeStackStack *
StackStackCreate(void)
{
	fungeStackStack * stackStack;
	fungeStack      * stack;

	stackStack = cf_malloc(sizeof(fungeStackStack) + sizeof(fungeStack*));
	if (!stackStack)
		return NULL;

	stack = StackCreate();
	if (!stack)
		return NULL;

	stackStack->size = 1;
	stackStack->current = 0;
	stackStack->stacks[0] = stack;
	return stackStack;
}

bool
StackStackBegin(instructionPointer * ip, fungeStackStack **me, size_t count, const fungePosition * storageOffset)
{
	fungeStackStack *stackStack;
	fungeStack      *newStack, *oldstack;

	oldstack = (*me)->stacks[(*me)->current];

	newStack = StackCreate();
	if (!newStack)
		return false;

	// Extend by one
	stackStack = cf_realloc(*me, sizeof(fungeStackStack) + ((*me)->size + 1) * sizeof(fungeStack*));
	if (!stackStack)
		return false;
	*me = stackStack;

	stackStack->size++;
	stackStack->stacks[stackStack->size] = newStack;
	stackStack->current = stackStack->size;

	if (count > 0) {
		// Dynamic array (C99) to copy elements into, because order must be preserved.
		FUNGEDATATYPE entriesCopy[count];
		for (size_t i = 0; i < count; i++)
			entriesCopy[i] = StackPop(oldstack);
		for (ssize_t i = count; i >= 0; i--)
			StackPush(entriesCopy[i], newStack);
	} else {
		while (count--)
			StackPush(0, newStack);
	}
	StackPushVector(&ip->storageOffset, newStack);
	ip->storageOffset.x = storageOffset->x;
	ip->storageOffset.y = storageOffset->y;
	ip->stack = newStack;
	ip->stackstack = stackStack;

	return true;
}


bool
StackStackEnd(fungeStackStack ** me)
{
	// TODO
}


bool
StackStackUnder(fungeStackStack ** me, size_t count)
{
	// TODO

}
