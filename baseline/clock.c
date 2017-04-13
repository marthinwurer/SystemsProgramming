/*
** SCCS ID:	@(#)clock.c	1.1	4/13/17
**
** File:	clock.c
**
** Author:	Warren R. Carithers and various CSCI-452 sections
**
** Contributor:
**
** Description:	Clock module implementation
*/

#define	__SP_KERNEL__

#include <x86arch.h>
#include <baseline/startup.h>

#include <baseline/common.h>

#include <baseline/clock.h>
#include <baseline/process.h>
#include <baseline/queue.h>
#include <baseline/scheduler.h>
#include <baseline/sio.h>
#include <baseline/syscall.h>

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// pinwheel control variables

static int32_t _pinwheel;	// pinwheel counter
static uint32_t _pindex;	// index into pinwheel string

/*
** PUBLIC GLOBAL VARIABLES
*/

time_t _now;		// the current system time

/*
** PRIVATE FUNCTIONS
*/


/*
** _clk_isr(vector,code)
**
** Interrupt handler for the clock module.  Spins the pinwheel,
** wakes up sleeping processes, and handles quantum expiration
** for the current process.
*/
static void _clk_isr( int vector, int code ) {
	pcb_t *pcb;

	// spin the pinwheel

	++_pinwheel;
	if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
		_pinwheel = 0;
		++_pindex;
		c_putchar_at( 79, 0, "|/-\\"[ _pindex & 3 ] );
	}

	// increment the system time

	++_now;

	/*
	** wake up any sleeper whose time has come
	**
	** we give awakened processes preference over the
	** current process (when it is scheduled again)
	*/

	status_t status = _q_peek( &_sleeping, (void **) &pcb );
	while( status == E_SUCCESS && pcb->wakeup <= _now ) {

		// time to wake up!  remove it from the queue
		status = _q_deque( &_sleeping, (void **) &pcb );
		if( status != E_SUCCESS || pcb == NULL ) {
			// it's there, but we can't remove it???
			_kpanic( "_clk_isr", "sleep queue remove failure" );
		}

		// and schedule it for dispatch
		_schedule( pcb );

		// see if there's another one to wake up
		status = _q_peek( &_sleeping, (void **) &pcb );
	}
	
	// check the current process to see if it needs to be scheduled

	// sanity check!

	_current->q_ticks -= 1;
	if( _current->q_ticks < 1 ) {
		_schedule( _current );
		_dispatch();
	}

#ifdef DUMP_QUEUES
	// Approximately every 10 seconds, dump the queues, and
	// print the contents of the SIO buffers.

	if( (_now % SECONDS_TO_TICKS(10)) == 0 ) {
		c_printf( "Queue contents @ %08x\n", _now );
		// should really iterate here
		// what if N_PRIOS != 4???
		_q_dump( "ready[0]", &_ready[0] );
		_q_dump( "ready[1]", &_ready[1] );
		_q_dump( "ready[2]", &_ready[2] );
		_q_dump( "ready[3]", &_ready[3] );
		_q_dump( "sleeping", &_sleeping );
		_q_dump( "waiting", &_waiting );
		_q_dump( "zombie", &_zombie );
		_q_dump( "reading", &_reading );
		_sio_dump();
	}
#endif

	// tell the PIC we're done

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _clk_init()
**
** initialize the clock module
*/

void _clk_init( void ) {
	uint32_t divisor;

        // announce that we got this far
        c_puts( " CLOCK" );

	// start the pinwheel

	_pinwheel = (CLOCK_FREQUENCY / 10) - 1;
	_pindex = 0;

	// return to the epoch

	_now = 0;

	// set the clock to tick at CLOCK_FREQUENCY Hz.

	divisor = TIMER_FREQUENCY / CLOCK_FREQUENCY;
	__outb( TIMER_CONTROL_PORT, TIMER_0_LOAD | TIMER_0_SQUARE );
        __outb( TIMER_0_PORT, divisor & 0xff );        // LSB of divisor
        __outb( TIMER_0_PORT, (divisor >> 8) & 0xff ); // MSB of divisor

	// register the ISR

	__install_isr( INT_VEC_TIMER, _clk_isr );
}
