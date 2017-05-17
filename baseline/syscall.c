/*
** SCCS ID:	@(#)syscall.c	1.1	4/13/17
**
** File:	syscall.c
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	system call module implementation
*/

#define	__SP_KERNEL__


#include <baseline/common.h>

#include <x86arch.h>
#include <uart.h>
#include <baseline/support.h>
#include <baseline/startup.h>

#include <baseline/syscall.h>
#include <baseline/scheduler.h>
#include <baseline/process.h>
#include <baseline/stack.h>
#include <baseline/clock.h>
#include <baseline/sio.h>

#include <stddef.h>


/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// system call jump table
//
// initialized by _sys_init() to ensure that the
// code::function mappings are correct

static void (*_syscalls[N_SYSCALLS])( pcb_t *pcb );

/*
** PUBLIC GLOBAL VARIABLES
*/

queue_t _zombie;	// the living dead
queue_t _waiting;	// concerned parents

/*
** PRIVATE FUNCTIONS
*/

/*
** _sys_isr(vector,code)
**
** Common handler for the system call module.  Selects
** the correct second-level routine to invoke based on
** the contents of EAX.
**
** The second-level routine is invoked with a pointer to
** the PCB for the process.  It is the responsibility of
** that routine to assign all return values for the call.
*/
void _sys_isr( int vector, int code ) {

	// cast away warnings
	(void) vector;
	(void) code;

	uint32_t syscode;
	
	// retrieve the system call code
	syscode = REG(_current,eax);
	
	// verify that it's valid
	if( syscode >= N_SYSCALLS ) {
		// nope!  fake it to be exit(badcode)
		syscode = SYS_exit;
		ARG(_current,1) = EXIT_BAD_CODE;
	}
	
	// invoke the second-level handler
	_syscalls[syscode]( _current );
	
	// tell the PIC we're done
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
}

/*
** Second-level syscall handlers
**
** All have this prototype:
**
**      static void _sys_NAME( pcb_t * );
**
** Values being returned to the user are placed into the EAX
** field in the context save area of the provided PCB.
*/

/*
** _sys_getpid - retrieve the PID of the current process
**
** implements:  pid_t getpid(void);
**
** returns:
**      the process' pid
*/
static void _sys_getpid( pcb_t *pcb ) {
	RET(pcb) = pcb->pid;
}

/*
** _sys_getppid - retrieve the parent PID of the current process
**
** implements:  pid_t getppid(void);
**
** returns:
**      the PID of the parent of this process
*/
static void _sys_getppid( pcb_t *pcb ) {
	RET(pcb) = pcb->ppid;
}

/*
** _sys_gettime - retrieve the current system time
**
** implements:  time_t gettime(void);
**
** returns:
**      the PID of the parent of this process
*/
static void _sys_gettime( pcb_t *pcb ) {
	RET(pcb) = _now;
}

/*
** _sys_getstatus - retrieve the state of the specified process
**
** implements:	state_t getstatus( pid_t pid );
**
** returns:
**	the state of the specified process, or PS_INVALID
*/
static void _sys_getstatus( pcb_t *pcb ) {
	pid_t pid;
	pcb_t *target;
	
	// locate the target
	pid = ARG(pcb,1);
	target = _pcb_find( pid );

	// either it's already gone, or it never existed
	if( target == NULL ) {
		RET(pcb) = PS_INVALID;
		return;
	}
	
	// found it!
	RET(pcb) = target->state;
}

/*
** _sys_exit - terminate the calling process
**
** implements:  void exit( int32_t status );
**
** does not return
*/
void _sys_exit( pcb_t *pcb ) {

	// grab the exit status
	int32_t status = ARG(pcb,1);
	
	// save it in the PCB
	pcb->exitstatus = status;

	// create the zombie!
	pcb->state = PS_ZOMBIE;
	_zombify( pcb );

	// select a new process to run
	_dispatch();
}

/*
** _sys_read - read from an input channel into a fixed-sized buffer
**
** implements:	int32_t read( uint32_t chan, char *buf, uint32_t size );
**
** blocks the calling routine if there is no character to return
**
** returns:
**	the character
*/
static void _sys_read( pcb_t *pcb ) {
	int n = 0;
	int chan  = ARG(pcb,1);
	char *buf = (char *) ARG(pcb,2);
	int size  = ARG(pcb,3);

	// try to get the next character

	switch( chan ) {

		case CHAN_CONSOLE:
			// console input doesn't block
			if( c_input_queue() < 1 ) {
				RET(pcb) = E_NO_CHAR;
				return;
			}
			n = c_gets( buf, size );
			break;

		case CHAN_SIO:
			// this may block
			n = _sio_reads( buf, size );
			break;

		default:
			// bad channel code!
			RET(pcb) = E_BAD_CHAN;
			return;
	}

	// if there was a character, return the count to the process;
	// otherwise, block the process until one comes in

	if( n > 0 ) {

		RET(pcb) = n;

	} else {

		// no character; put this process on the
		// serial i/o input queue

		status_t stat = _q_enque( &_reading, pcb );
		if( stat != E_SUCCESS ) {
			RET(pcb) = E_FAILURE;
			return;
		}

		// mark it as blocked

		_current->state = PS_BLOCKED;

		// select a new current process

		_dispatch();

	}

}

/*
** _sys_write - write a sized buffer to a channel
**
** implements:	int32_t write( uint32_t chan, char *buf, uint32_t size );
**
** returns:
**	status of the operation
*/
static void _sys_write( pcb_t *pcb ) {
	int chan  = ARG(pcb,1);
	char *buf = (char *) ARG(pcb,2);
	int size  = ARG(pcb,3);

	// this is almost insanely simple, but it does separate
	// the low-level device access fromm the higher-level
	// syscall implementation

	switch( chan ) {

		case CHAN_CONSOLE:
			// the c_io module doesn't have a
			// "write N characters" function
			for( int i = 0; i < size; ++i ) {
				c_putchar( buf[i] );
			}
			RET(pcb) = size;
			break;

		case CHAN_SIO:
			// much simpler!
			_sio_writes( buf, size );
			RET(pcb) = size;
			break;

		default:
			RET(pcb) = E_BAD_CHAN;
			break;

	}

}

/*
** _sys_sleep - put the current process to sleep for some length of time
**
** implements:  void sleep( uint32_t msec );
*/
void _sys_sleep( pcb_t *pcb ) {
	
	// retrieve the desired sleep time
	uint32_t msec = ARG(pcb,1);
	
	if( msec == 0 ) {

		// for a value of 0, just yield the CPU
		_schedule( pcb );

	} else {

		// calculate sleep duration in ticks
		msec = MS_TO_TICKS(msec);

		// determine wakeup time
		pcb->wakeup = _now + msec;

		// mark it as a sleeper, and add it to the queue
		pcb->state = PS_SLEEPING;
		status_t status = _q_enque( &_sleeping, (void *) pcb );
		if( status != E_SUCCESS ) {
			_kpanic( "_sys_sleep", "can't enque sleeper" );
		}
	}
	
	// need a new running process
	_dispatch();
}

/*
** _sys_kill - terminate a process with extreme prejudice
**
** implements:  int32_t kill( pid_t pid );
**
** returns:
**	E_SUCCESS if found
**	E_NO_PROCESS if not found
*/
void _sys_kill( pcb_t *pcb ) {

	// get the victim's PID
	pid_t pid = ARG(pcb,1);
	
	// locate it's PCB
	pcb_t *victim = _pcb_find( pid );
	if( victim == NULL ) {
		RET(pcb) = E_NO_PROCESS;
		return;
	}
	
	// found it; figure out where it is in the system

	queue_t *which = NULL;

	switch( victim->state ) {

		case PS_WAITING:
			which = &_waiting;
			break;
		
		case PS_SLEEPING:
			which = &_sleeping;
			break;
		
		case PS_BLOCKED: // baseline system: reading is the only one
			which = &_reading;
			break;
		
		case PS_READY:
			which = &_ready[ victim->prio ];
			break;
		
		case PS_RUNNING:
			// ack!  it's us!
			break;
			
		case PS_KILLED:
		case PS_ZOMBIE:
			// if it's already killed, nothing more to do
			RET(pcb) = E_SUCCESS;
			return;

		default:
			// uh-oh - there shouldn't be any other
			// possible states, so...
			c_printf( "*** _sys_kill, actor PID %d", pcb->pid );
			c_printf( ", victim PID %d state %d\n",
				  victim->pid, victim->state );
			_kpanic( "_sys_kill", "bad victim state" );
	}
	
	// if it isn't us, pull it off its queue
	if( which != NULL ) {
		status_t status = _q_remove( which, victim );
		if( status != E_SUCCESS ) {
			_kpanic( "_sys_kill", "can't remove victim" );
		}
	}
	
	// mark it as deceased
	victim->state = PS_KILLED;
	victim->exitstatus = EXIT_KILLED;

	// we'll feel funny if it's us, I guess
	RET(pcb) = E_SUCCESS;

	// determine the final disposition of this process
	_zombify( victim );
		
	// if it was us, we no longer have a running process, so pick one
	if( victim == _current ) {
		_dispatch();
	}
}

/*
** _sys_wait - wait for one of the children of a process
**
** implements:  int32_t wait( pid_t *pid, int32_t *status );
**
** returns:
**	on error, E_NO_CHILD
**	on success, E_SUCCESS, and the pid and status of the child
*/
void _sys_wait( pcb_t *pcb ) {

	// if there aren't any children, nothing to do!
	if( pcb->children == 0 ) {
		RET(pcb) = E_NO_CHILD;
		return;
	}

	// get the pointers to the 'out' parameters
	pid_t *pidptr = (pid_t *) ARG(pcb,1);
	int32_t *statptr = (int32_t *) ARG(pcb,2);
	
	// find the first child that's a zombie
	pcb_t *child = _pcb_find_child_of( pcb->pid );
	while( child != NULL ) {
		if( child->state == PS_ZOMBIE ) {
			// found one!  remove it from the zombie queue
			status_t status = _q_remove( &_zombie, child );
			if( status != E_SUCCESS ) {
				_kpanic( "_sys_wait", "can't deque zombie" );
			}

			// return its information to the parent
			RET(pcb) = E_SUCCESS;
			pcb->children -= 1;
			*pidptr = child->pid;
			*statptr = child->exitstatus;

			// clean up the zombie
			_pcb_cleanup( child );

			// all done!
			return;
		} else {
			// look for another child of this parent
			child = _pcb_find_child_of( -1 );
		}
	}

	// no zombie children, so must wait
	pcb->state = PS_WAITING;
	status_t status = _q_enque( &_waiting, (void *) pcb );
	if( status != E_SUCCESS ) {
		_kpanic( "_sys_wait", "can't enque waiting parent" );
	}

	// need a new running process
	_dispatch();
}

/*
** _sys_fork - create a new process
**
** implements:  pid_t fork( void );
**
** returns:
**	parent: PID of new child, or -1 on error
**	child:  0
*/
static void _sys_fork( pcb_t *pcb ) {
	pcb_t *new;
	stack_t *stk;
	
	// verify that we haven't run out of PIDs

	if( _next_pid >= 65000U ) {
		RET(pcb) = E_NO_MORE_PROCS;
		c_printf( "*** _next_pid is %u\n", _next_pid );
		c_puts( "*** No more processes can be created\n" );
		return;
	}
	
	// allocate the PCB and stack

	new = _pcb_alloc();
	if( new == NULL ) {
		RET(pcb) = E_NO_PCBS;
		return;
	}
	
	stk = _stk_alloc();
	if( stk == NULL ) {
		RET(pcb) = E_NO_STACKS;
		_pcb_dealloc( new );
		return;
	}
	
	// duplicate the parent's PCB
	_kmemcpy( (uint8_t *) new, (uint8_t *) pcb, sizeof(pcb_t) );

	// fix the fields that need to be changed
	new->pid = _next_pid++;
	new->ppid = pcb->pid;
	new->q_default = QUANT_STD;
	new->stack = stk;
	
	// now, duplicate the parent's stack
	_kmemcpy( (uint8_t *) stk, (uint8_t *) (pcb->stack), sizeof(stack_t) );
	
	// figure out the byte offset from stack to stack
	int32_t offset = (int)
	    ( ((ptrdiff_t)new->stack) - ((ptrdiff_t)pcb->stack) );
	
	// fix the child's context pointer
	new->context = (context_t *)
	    ( ((ptrdiff_t) pcb->context) + offset );
	
	// fix the child's EBP
	REG(new,ebp) += offset;
	
	// fix the child's memory pointer
	new->memory = pcb->memory;

	// return values for the two processes
	RET(new) = 0;
	RET(pcb) = new->pid;
	pcb->children += 1;
	
	// schedule the child, let the parent continue executing
	_schedule( new );
}

/*
** _sys_exec - replace this program with a different one
**
** implements:  int32_t exec( int (*entry)(void *), void *arg, prio_t prio );
**
** returns:
**	on error, E_NO_PROCESS
**	on success, doesn't return
*/
static void _sys_exec( pcb_t *pcb ) {
	uint32_t entry = ARG(pcb,1);
	void *arg = (void *) ARG(pcb,2);
	
	// reset the process' priority
	pcb->prio = (prio_t) ARG(pcb,3);
	
	// clean up the stack and set the new entry point
	pcb->context = _stk_setup( pcb->stack, entry, arg );

	// get the process a new address space and switch to it.
	// if the process is system priority, give it the identity mapped pde
	if( pcb->prio == 0){
		pcb->memory = get_identity_mapped();
	}else{
		pcb->memory = new_page_directory();
	}
	c_printf("exec: PID: %d, PDE: %x\n", pcb->pid, pcb->memory);

//	set_page_directory(pcb->memory);

	// return back to its new memory space
	set_return_pde(pcb->memory);

	// question:  should we schedule and dispatch here?
	// we choose not to, so the "new" program will
	// begin executing immediately, with whatever's left
	// of the "old" quantum for the process.
}

/*
 * _sys_mmap - map the given memory to the current memory space
 *
 * implements:
 */
static void _sys_mmap( pcb_t *pcb ) {
	address_space_t current = pcb->memory;
	void * arg1 = (void *) ARG(pcb,1);
	void * arg2 = (void *) ARG(pcb,2);
	size_t arg3 = (size_t) ARG(pcb,3);

	int status = 0;


	c_printf("_sys_mmap %x, %x, %x, %x\n", current, arg1, arg2, arg3);

	_k_mmap(current, arg1, arg2, arg3, &status);

	RET(pcb) = status;
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _sys_init()
**
** initialize the syscall module
*/

void _sys_init( void ) {

	// report that we made it this far
	c_puts( " SYSCALL" );

	/*
	** Set up the syscall jump table.  We do this here
	** to ensure that the association between syscall
	** code and function address is correct even if the
	** codes change.
	*/

	_syscalls[ SYS_exit ]		= _sys_exit;
	_syscalls[ SYS_read ]		= _sys_read;
	_syscalls[ SYS_write ]		= _sys_write;
	_syscalls[ SYS_fork ]		= _sys_fork;
	_syscalls[ SYS_exec ]		= _sys_exec;
	_syscalls[ SYS_kill ]		= _sys_kill;
	_syscalls[ SYS_wait ]		= _sys_wait;
	_syscalls[ SYS_sleep ]		= _sys_sleep;
	_syscalls[ SYS_getpid ]		= _sys_getpid;
	_syscalls[ SYS_getppid ]	= _sys_getppid;
	_syscalls[ SYS_gettime ]	= _sys_gettime;
	_syscalls[ SYS_getstatus ]	= _sys_getstatus;
	_syscalls[ SYS_mmap ]	= _sys_mmap;

	// initialize the zombie and waiting queues
	_q_reset( &_zombie, NULL );
	_q_reset( &_waiting, NULL );

	// install the second-stage ISR
	__install_isr( INT_VEC_SYSCALL, _sys_isr );
}
