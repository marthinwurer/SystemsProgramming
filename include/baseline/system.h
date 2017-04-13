/*
** SCCS ID:	@(#)system.h	1.1	4/13/17
**
** File:	system.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	miscellaneous OS support functions
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "common.h"
#include <x86arch.h>
#include "bootstrap.h"

#include "process.h"

// copied from ulib.h
void do_exit( void );

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

// default contents of EFLAGS register

#define DEFAULT_EFLAGS  (EFLAGS_MB1 | EFLAGS_IF)

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _zombify(pcb)
**
** turn a process into a zombie, or give its status to
** a waiting parent
*/
void _zombify( pcb_t *pcb );

/*
** _init - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/
void _init( void );

#endif

#endif
