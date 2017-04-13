/*
** SCCS ID:	@(#)clock.h	1.1	4/13/17
**
** File:	clock.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	Clock module declarations
*/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// standard process quantum (in ticks)

#define	QUANT_STD		5

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

extern uint32_t _now;   // the current system time

/*
** Prototypes
*/

/*
** _clk_init()
**
** initialize the clock module
*/
void _clk_init( void );

#endif

#endif
