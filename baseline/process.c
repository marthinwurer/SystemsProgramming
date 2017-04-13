/*
** SCCS ID:	@(#)process.c	1.1	4/13/17
**
** File:	process.c
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	implementation of the process module
*/

#define	__SP_KERNEL__

#include <baseline/common.h>

#include <baseline/process.h>
#include <baseline/queue.h>

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// all unallocated PCBs

static queue_t _free_pcbs;

/*
** PUBLIC GLOBAL VARIABLES
*/

pcb_t _pcbs[N_PCBS];	// all the PCBs in the system
uint16_t _next_pid;	// next available PID

// need to declare this somewhere!

queue_t _sleeping;	// sleeping processes

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _pcb_init()
**
** initializes all PCB-related data structures
**
** Should only be called when there are no active processes!
*/
void _pcb_init( void ) {

	// announce our presence
	c_puts( " PCBS" );

	// ensure that the free queue is completely empty
	_q_reset( &_free_pcbs, NULL );

	// also initialize the sleep queue (ordered by wakeup time)
	_q_reset( &_sleeping, _cmp_wakeup );
	
	// free the PCBs!
	for( int i = 0; i < N_PCBS; ++i ) {
		_pcb_dealloc( &_pcbs[i] );
	}

	// start PIDs out at a known value
	_next_pid = PID_FIRST_USER;
}

/*
** _pcb_cleanup(pcb)
**
** clean up and free a pcb and its associated stack
*/
void _pcb_cleanup( pcb_t *pcb ) {

	// if there is a stack, release it
	if( pcb->stack != NULL ) {
		_stk_dealloc( pcb->stack );
	}

	// now release the PCB
	_pcb_dealloc( pcb );
}

/*
** _pcb_alloc()
**
** allocate a PCB structure
**
** returns a pointer to the PCB, or NULL on failure
*/
pcb_t *_pcb_alloc( void ) {
	pcb_t *new;
	
	// if there aren't any, nothing to do here
	if( _q_size(&_free_pcbs) < 1 ) {
		return( NULL );
	}
	
	// pull the first free PCB from the queue
	status_t status = _q_deque( &_free_pcbs, (void **) &new );
	if( status != E_SUCCESS || new == NULL ) {
		_kpanic( "_pcb_alloc", "no free PCBs but size >= 1" );
	}
	
	// ensure this PCB is seen as "in use"
	new->state = PS_NEW;
	
	return( new );
}

/*
** _pcb_dealloc(pcb)
**
** deallocate a PCB, putting it into the list of available PCBs
*/
void _pcb_dealloc( pcb_t *pcb ) {

	// can't deallocate nothing!
	if( pcb == NULL ) {
		return;
	}
	
	// mark the PCB as available
	pcb->state = PS_FREE;

	// ensure that it can't be found by a PID or PPID search
	pcb->pid = -1; // 20170328
	pcb->ppid = -1;

	// put it back on the free PCB queue
	_q_enque( &_free_pcbs, (void *) pcb );
}

/*
** _pcb_find(pid)
**
** locate the PCB having the specified PID
**
** returns a pointer to the PCB, or NULL if not found
*/
pcb_t *_pcb_find( pid_t pid ) {
	pcb_t *pcb;
	
	// very simple:  iterate through the set of PCBs
	// looking for one with the specified PID
	for( pcb = &_pcbs[0]; pcb < &_pcbs[N_PCBS]; ++pcb ) {
		if( pid == pcb->pid ) {
			return( pcb );
		}
	}
	
	// he's dead, jim
	return( NULL );
}

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
pcb_t *_pcb_find_child_of( int32_t ppid ) {
	static pcb_t *curr = NULL;
	static int target = -1;
	
	// can't pick up a search that hasn't started
	if( ppid == -1 && curr == NULL ) {
		_kpanic( "_pcb_find_child_of", "ppid -1 but curr NULL" );
	}
	
	if( ppid != -1 || curr == NULL ) {
		// we must search from the beginning
		curr = &_pcbs[0];
		target = ppid;
	} else {
		// pick up where we left off (skipping
		// the previously-returned PCB)
		++curr;
	}
	
	// see if we can find it
	while( curr < &_pcbs[N_PCBS] ) {
		if( target == curr->ppid ) {
			return( curr );
		}
		++curr;
	}
	
	// nope; we ran off the end of the world, so
	// we can't just continue this search
	curr = NULL;
	target = -1;

	return( NULL );
}

/*
** _pcb_dump(pcb)
**
** dump the contents of this PCB to the console
*/
void _pcb_dump( const char *which, pcb_t *pcb ) {

	c_printf( "%s @ %08x: ", which, (uint32_t) pcb );
	if( pcb == NULL ) {
		c_puts( " NULL???\n" );
		return;
	}

	c_printf( " pids %d/%d state ", pcb->pid, pcb->ppid );
	switch( pcb->state ) {
		case PS_INVALID:	c_puts( "INV" ); break;
		case PS_NEW:		c_puts( "NEW" ); break;
		case PS_READY:		c_puts( "RDY" ); break;
		case PS_RUNNING:	c_puts( "RUN" ); break;
		case PS_SLEEPING:	c_puts( "SLP" ); break;
		case PS_WAITING:	c_puts( "WTG" ); break;
		case PS_BLOCKED:	c_puts( "BLK" ); break;
		case PS_KILLED:		c_puts( "KIL" ); break;
		case PS_ZOMBIE:		c_puts( "ZOM" ); break;
		default:		c_printf( "? (%d)", pcb->state );
	}

	c_printf( "\n wake %08x xit %d", pcb->wakeup,
		pcb->exitstatus );

	c_printf( " kids %d prio %d Q %d of %d",
		pcb->children, pcb->prio, pcb->q_ticks, pcb->q_default );

	c_printf( " context %08x stack %08x\n",
		  (uint32_t) pcb->context, (uint32_t) pcb->stack );
}

/*
** _context_dump(context)
**
** dump the contents of this context to the console
*/
void _context_dump( const char *which, context_t *context ) {

	c_printf( "%s @ %08x: ", which, (uint32_t) context );
	if( context == NULL ) {
		c_puts( " NULL???\n" );
		return;
	}

	c_printf( "\n     ss %08x  gs %08x  fs %08x  es %08x\n",
		context->ss, context->gs, context->fs, context->es );
	c_printf( "     ds %08x edi %08x esi %08x ebp %08x\n",
		context->ds, context->edi, context->esi, context->ebp );
	c_printf( "    esp %08x ebx %08x edx %08x ecx %08x\n",
		context->esp, context->ebx, context->edx, context->ecx );
	c_printf( "    eax %08x vec %08x cod %08x eip %08x\n",
		context->eax, context->vector, context->code, context->eip );
	c_printf( "     cs %08x efl %08x\n",
		context->cs, context->eflags );
}
