/*
** SCCS ID:	@(#)types.h	1.1	4/13/17
**
** File:	types.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	General data type declarations
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#include "common.h"
#include <stdint.h>

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Types
*/

// size-specific integer types
//
//typedef char		int8_t;
//typedef unsigned char	uint8_t;
//
//typedef short		int16_t;
//typedef unsigned short	uint16_t;
//
//typedef int		int32_t;
//typedef unsigned int	uint32_t;

// specialized type names

typedef uint32_t	time_t;
typedef int16_t	pid_t;

// status types

typedef enum e_stat {

	// returnable to users
	E_NO_PCBS = -9, E_NO_STACKS = -8, E_NO_MORE_PROCS = -7,
	E_NO_CHAR = -6, E_NO_CHILD = -5, E_BAD_CHAN = -4,
	E_NO_PROCESS = -3, E_NO_PERM = -2, E_FAILURE = -1,
	E_SUCCESS = 0,
    
    E_NOT_IMPLEMENTED = -50,
    E_BAD_TYPE = -51,
    E_NO_MORE_HANDLES = -52,
    E_BAD_HANDLE = -53,
    E_NO_DATA = -54,
    E_OUT_OF_BOUNDS = -55, //Provided index out of range
    E_BAD_ARG = -56, //Bad argument provided to IO function
    E_NO_MATCH = -57, //No Match for Path in IO operation
    E_CANCELED = -58, //IO Operation In Progress Canceled by Middleware
    E_LOCKED = -59,
    E_BAD_PTR = -60,
    E_OUT_OF_OBJECTS = -61, //no more objects available for allocation
    E_MORE_DATA = -62,
	// used only within the OS
	E_NO_QNODES, E_EMPTY_QUEUE

} status_t;

// alias for E_SUCCESS
#define	SUCCESS		E_SUCCESS

// process priorities

typedef enum e_prio {
	P_SYSTEM = 0, P_HIGH = 1, P_LOW = 2, P_MIN = 3
} prio_t;

#define N_PRIOS		4

// process states

typedef enum e_state {
	PS_INVALID = -1,
	PS_FREE = 0, PS_NEW, PS_READY, PS_RUNNING, PS_SLEEPING,
	PS_WAITING, PS_BLOCKED, PS_KILLED, PS_ZOMBIE
} state_t;

// the "active process" range of states

#define PS_FIRST_LIVE	PS_NEW
#define	PS_LAST_LIVE	PS_BLOCKED

#ifdef __SP_KERNEL__

/*
** OS-only type declarations
*/

#endif

#endif

#endif
