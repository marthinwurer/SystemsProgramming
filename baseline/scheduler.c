/*
** SCCS ID:	@(#)scheduler.c	1.1	4/13/17
**
** File:	scheduler.c
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	implementation of the scheduler module
*/

#define	__SP_KERNEL__

#include <baseline/common.h>

#include <baseline/scheduler.h>
#include <baseline/process.h>
#include <baseline/queue.h>
#include <baseline/clock.h>

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

pcb_t *_current;        // the currently-running process
pcb_t *_init_pcb;       // PCB for the init process

queue_t _ready[N_PRIOS];	// the MLQ

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _sched_init()
**
** initialize the scheduler module
*/
void _sched_init( void ) {

	// we made it this far!
	c_puts( " SCHED" );
	
	// reset all the ready queues
	for( int i = 0; i < N_PRIOS; ++i ) {
		_q_reset( &_ready[i], NULL );
	}
}

/*
** _schedule(pcb)
**
** schedule a process for execution according to its priority
*/
void _schedule( pcb_t *pcb ) {

	// make sure there's actually something to schedule
	if( pcb == NULL ) {
		return;
	}
	
	// make sure the priority is legal
	if( pcb->prio < P_SYSTEM || pcb->prio > P_MIN ) {
		pcb->prio = P_LOW;
	}
	
	// all it needs now is a CPU....
	pcb->state = PS_READY;

	// add it to the appropriate queue
	status_t status = _q_enque( &_ready[pcb->prio], (void *) pcb );
	if( status != E_SUCCESS ) {
		_kpanic( "_schedule", "can't enque ready PCB" );
	}
}

/*
** _dispatch()
**
** give the CPU to a process
*/
void _dispatch( void ) {
	prio_t prio;
	
	// find a non-empty ready queue
	for( prio = P_SYSTEM; prio <= P_MIN; ++prio ) {
		if( _q_size(&_ready[prio]) > 0 ) {
			break;
		}
	}
	
	// if we didn't find one, we're in trouble
	if( prio > P_MIN ) {
		_kpanic( "_dispatch", "no ready processes!?!?!" );
	}
	
	// found one - pull out the first entry
	status_t status = _q_deque( &_ready[prio], (void **) &_current );
	if( status != E_SUCCESS || _current == NULL ) {
		_kpanic( "_dispatch", "can't deque ready PCB" );
	}

	// prepare to turn it loose
	_current->state = PS_RUNNING;
	_current->q_ticks = _current->q_default;

	// change the virtual memory
//	c_printf("dispatch: PID: %d, PDE: %x\n", _current->pid, _current->memory);
	set_return_pde(_current->memory);

//	c_printf("last: %x\n\n", get_return_pde());
//	c_printf("Current: %x\n\n", get_current_pde());

}	
