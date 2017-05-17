/*
** SCCS ID:	%W%	%G%
**
** File:	common.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	declarations needed by the OS and by user code
*/

#ifndef _COMMON_H_
#define _COMMON_H_

/*
** General (C and/or assembly) definitions
*/

// correct (?) way to define NULL
#ifndef NULL
#define NULL		((void *) 0)
#endif
// maximum number of processes the system will support

#define	N_PROCS		25

// i/o channels

#define	CHAN_CONSOLE	0
#define	CHAN_SIO	1

// predefined exit status values

#define	EXIT_SUCCESS	0
#define	EXIT_FAILURE	1
#define	EXIT_BAD_CODE	2
#define	EXIT_KILLED	3

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

// clock interrupts per second

#define	CLOCK_FREQUENCY		1000

// pseudo function to convert seconds to milliseconds

#define SECONDS_TO_MS(n)	((n) * 1000)

// pseudo function to convert milliseconds to clock ticks
// (currently, a no-op, as the base clock rate is 1000 ticks/sec)

#define MS_TO_TICKS(n)	((n))

// pseudo function to convert seconds to ticks

#define SECONDS_TO_TICKS(n)	(MS_TO_TICKS(SECONDS_TO_MS((n))))

// pseudo function to convert ticks to (truncated) seconds

#define TICKS_TO_SECONDS(n)	((n) / CLOCK_FREQUENCY)

// pseudo function to convert ticks to (rounded up) seconds

#define TICKS_TO_ROUNDED_SECONDS(n)	(((n)+(CLOCK_FREQUENCY-1)) / CLOCK_FREQUENCY)

// only pull these in if we're not in assembly language

#include "types.h"

#ifdef __SP_KERNEL__

// OS needs the system headers and the kernel library headers

#include "c_io.h"
#include "support.h"
#include "system.h"

#include "klib.h"

#else

// User code needs only the user library headers

#include "ulib.h"

#endif

#endif

#endif
