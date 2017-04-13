/*
** SCCS ID:	@(#)stack.h	1.1	4/13/17
**
** File:	stack.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	definitions and prototypes for the stack module
*/

#ifndef _STACK_H_
#define _STACK_H_

#include "common.h"

#include "process.h"

/*
** General (C and/or assembly) definitions
*/

// number of user stacks, including ones for the idle process,
// the initial user process, and the OS

#define	N_STACKS	(N_PROCS + 3)

// number of 32-bit unsigned integers in a stack

#define	STACK_WORDS	1024

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Types
*/

// stack structure

typedef uint32_t stack_t[STACK_WORDS];

/*
** Globals
*/

extern stack_t _system_stack;	// the OS stack
extern uint32_t *_system_esp;	// the OS %ESP value

/*
** Prototypes
*/

/*
** _stk_init()
**
** initializes all stack-related data structures
*/
void _stk_init( void );

/*
** _stk_alloc()
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/
stack_t *_stk_alloc( void );

/*
** _stk_dealloc(stk)
**
** deallocate a stack, putting it into the list of available stacks
*/
void _stk_dealloc( stack_t *stk );

/*
** _stk_dump(msg,stk)
**
** dump the contents of the provided stack, eliding
** duplicate lines
*/
void _stk_dump( char *msg, stack_t *stack );

/*
** _stk_setup(stack,entry,arg)
**
** set up 'stack' with a fresh user context
**
** returns a pointer to the context save area in the stack
*/
context_t *_stk_setup( stack_t *stack, uint32_t entry, void *arg );

#endif

#endif
