/*
** SCCS ID:	@(#)process.h	1.1	4/13/17
**
** File:	process.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	definitions and prototypes for the process module
*/

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "common.h"
#include <kern/memory/memory_map.h>

/*
** General (C and/or assembly) definitions
*/

#define	N_PCBS	(N_PROCS+3)

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

#ifdef __SP_KERNEL__

/*
** OS-only declarations
*/

// PID of the initial user process

#define	PID_INIT	1

// first 'user' PID

#define	PID_FIRST_USER	100

// REG(pcb,x) -- access  a specific register in a process context

#define	REG(pcb,x)	((pcb)->context->x)

// RET(pcb) -- access return value register in process context

#define	RET(pcb)	((pcb)->context->eax)

// ARG(pcb,n) -- access argument #n from the indicated process
//
// ARG(pcb,0) --> return address
// ARG(pcb,1) --> first actual parameter
// ARG(pcb,2) --> second actual parameter
//
// IF THE PARAMETER PASSING MECHANISM CHANGES, SO MUST THIS

#define	ARG(pcb,n)	( ( (uint32_t *) (((pcb)->context) + 1) ) [(n)] )

/*
** Types
*/

///
// process context structure
//
// NOTE:  the order of data members here depends on the
// register save code in isr_stubs.S!!!!
///

typedef struct context {
	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t vector;
	uint32_t code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} context_t;

// include these here after the context definition

#include "stack.h"
#include "queue.h"

///
// process control block
//
// members are ordered by size
///

typedef struct pcb {
	// 32-bit fields
	context_t	*context;	// context save area pointer
	stack_t		*stack;		// per-process runtime stack
	time_t		wakeup;		// for sleeping processes
	int32_t		exitstatus;	// holds state of zombie
	address_space_t	memory; // the virtual memory context for this process

	// 16-bit fields
	pid_t		pid;		// our pid
	pid_t		ppid;		// out parent's pid
	uint16_t	children;	// # of currently-active children

	// 8-bit fields
	state_t		state;		// current process state
	prio_t		prio;		// scheduling priority
	uint8_t		q_default;	// default quantum for this proc
	uint8_t		q_ticks;	// remaining execution quantum
} pcb_t;

/*
** Globals
*/

extern pcb_t _pcbs[];		// all PCBs in the system
extern uint16_t _next_pid;	// next available PID

// need to declare this somewhere!

extern queue_t _sleeping;	// sleeping processes

/*
** Prototypes
*/

/*
** _pcb_init()
**
** initializes all PCB-related data structures
*/
void _pcb_init( void );

/*
** _pcb_cleanup(pcb)
**
** clean up and free a pcb and its associated stack
*/
void _pcb_cleanup( pcb_t *pcb );

/*
** _pcb_alloc()
**
** allocate a PCB structure
**
** returns a pointer to the PCB, or NULL on failure
*/
pcb_t *_pcb_alloc( void );

/*
** _pcb_dealloc(pcb)
**
** deallocate a PCB, putting it into the list of available PCBs
*/
void _pcb_dealloc( pcb_t *pcb );

/*
** _pcb_find(pid)
**
** locate the PCB having the specified PID
**
** returns a pointer to the PCB, or NULL if not found
*/
pcb_t *_pcb_find( pid_t pid );

/*
** _pcb_find_child_of(ppid)
**
** locate the next PCB having the specified PPID
**
** operates as an iterator - subsequent calls with -1 argument
** pick up where the previous call left off
**
** returns a pointer to the PCB, or NULL if not found
*/
pcb_t *_pcb_find_child_of( int32_t ppid );

/*
** _pcb_dump(pcb)
**
** dump the contents of this PCB to the console
*/
void _pcb_dump( const char *which, pcb_t *pcb );

/*
** _context_dump(pcb)
**
** dump the contents of this process context to the console
*/
void _context_dump( const char *which, context_t *context );

#endif

#endif

#endif
