/*
** SCCS ID:	@(#)user.c	1.1	4/13/17
**
** File:	user.c
**
** Author:	Warren R. Carithers and various CSCI-452 classes
**
** Contributor:
**
** Description:	user routines
*/

#include <baseline/common.h>

#include <baseline/user.h>

#include <kern/net/net_test.h>
#include <kern/net/intel.h>
#include <baseline/c_io.h>

/*
** Support functions
*/

/*
** report(ch,arg,pid)
**
** report that user 'ch' is running as 'pid' with 'arg'
*/
void report( char ch, void *arg, pid_t pid ) {
	int i;
	char buf[16];

	cwrites( "User " );
	cwritech( ch );
	cwrites( "(" );
	i = cvt_dec( buf, (int) arg );
	cwrite( buf, i );
	cwrites( "), pid " );
	i = cvt_dec( buf, pid );
	cwrite( buf, i );
	cwrites( ", running\n" );
}

/*
** USER PROCESSES
**
** Each is designed to test some facility of the OS; see the user.h
** header file for a summary of which system calls are tested by 
** each user function.
**
** Output from user processes is always alphabetic.  Uppercase 
** characters are "expected" output; lowercase are "erroneous"
** output.
**
** More specific information about each user process can be found in
** the header comment for that function (below).
**
** To spawn a specific user process, uncomment its SPAWN_x
** definition in the user.h header file.
*/

/*
** Prototypes for all one-letter user main routines (even
** ones that may not exist, for completeness)
*/

int32_t user_a( void * ); int32_t user_b( void * ); int32_t user_c( void * );
int32_t user_d( void * ); int32_t user_e( void * ); int32_t user_f( void * );
int32_t user_g( void * ); int32_t user_h( void * ); int32_t user_i( void * );
int32_t user_j( void * ); int32_t user_k( void * ); int32_t user_l( void * );
int32_t user_m( void * ); int32_t user_n( void * ); int32_t user_o( void * );
int32_t user_p( void * ); int32_t user_q( void * ); int32_t user_r( void * );
int32_t user_s( void * ); int32_t user_t( void * ); int32_t user_u( void * );
int32_t user_v( void * ); int32_t user_w( void * ); int32_t user_x( void * );
int32_t user_y( void * ); int32_t user_z( void * );

/*
** Users A, B, and C are identical, except for the character they
** print out via swritech().  Each prints its ID, then loops 30
** times delaying and printing, before exiting.  They also verify
** the status return from swrite().
*/

int32_t user_a( void *arg ) {
	int i, j;
	int32_t n;
	char buf[12];
	// cast away warnings
	(void) arg;

	n = swritech( 'A' );
	if( n != 1 ) {
		cwrites( "User A, write 1 returned " );
		i = cvt_dec( buf, n );
		cwrite( buf, i );
		cwritech( '\n' );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		n = swritech( 'A' );
		if( n != 1 ) {
			cwrites( "User A, write 2 returned " );
			i = cvt_dec( buf, n );
			cwrite( buf, i );
			cwritech( '\n' );
		}
	}

	exit( EXIT_SUCCESS );

	n = swritech( 'a' );	/* shouldn't happen! */
	if( n != 1 ) {
		cwrites( "User A, write 3 returned " );
		i = cvt_dec( buf, n );
		cwrite( buf, i );
		cwritech( '\n' );
	}

	return( 0 );  // shut the compiler up!

}

int32_t user_b( void *arg ) {
	int i, j;
	int32_t n;
	char buf[12];
	// cast away warnings
	(void) arg;


	n = swritech( 'B' );
	if( n != 1 ) {
		cwrites( "User B, write 1 returned " );
		i = cvt_dec( buf, n );
		cwrite( buf, i );
		cwritech( '\n' );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		n = swritech( 'B' );
		if( n != 1 ) {
			cwrites( "User B, write 2 returned " );
			i = cvt_dec( buf, n );
			cwrite( buf, i );
			cwritech( '\n' );
		}
	}

	exit( EXIT_SUCCESS );

	n = swritech( 'b' );	/* shouldn't happen! */
	if( n != 1 ) {
		cwrites( "User B, write 3 returned " );
		i = cvt_dec( buf, n );
		cwrite( buf, i );
		cwritech( '\n' );
	}
	return( 0 );  // shut the compiler up!

}

int32_t user_c( void *arg ) {
	// cast away warnings
	(void) arg;

	int i, j;
	int32_t n;
	char buf[12];

	n = swritech( 'C' );
	if( n != 1 ) {
		cwrites( "User C, write 1 returned " );
		i = cvt_dec( buf, n );
		cwrite( buf, i );
		cwritech( '\n' );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		n = swritech( 'C' );
		if( n != 1 ) {
			cwrites( "User C, write 2 returned " );
			i = cvt_dec( buf, n );
			cwrite( buf, i );
			cwritech( '\n' );
		}
	}

	exit( EXIT_SUCCESS );

	n = swritech( 'c' );
	if( n != 1 ) {
		cwrites( "User C, write 3 returned " );
		i = cvt_dec( buf, n );
		cwrite( buf, i );
		cwritech( '\n' );
	}
	return( 0 );  // shut the compiler up!

}


/*
** User D spawns user Z, then exits before it can terminate.
*/

int32_t user_d( void *arg ) {
	// cast away warnings
	(void) arg;

	pid_t pid;
	int ret = EXIT_SUCCESS;

	swritech( 'D' );

	pid = spawn( user_z, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "User D spawn() failed\n" );
		ret = EXIT_FAILURE;
	}

	swritech( 'D' );

	exit( ret );

	return( 0 );  // shut the compiler up!

}


/*
** Users E, F, and G test the sleep facility.
**
** User E sleeps for 10 seconds at a time.
*/

int32_t user_e( void *arg ) {
	// cast away warnings
	(void) arg;


	report( 'E', arg, getpid() );
	swritech( 'E' );
	for( int i = 0; i < 5 ; ++i ) {
		sleep( SECONDS_TO_MS(10) );
		swritech( 'E' );
	}

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}

/*
** User F sleeps for 5 seconds at a time.
*/

int32_t user_f( void *arg ) {
	// cast away warnings
	(void) arg;


	report( 'F', arg, getpid() );
	swritech( 'F' );
	for( int i = 0; i < 5 ; ++i ) {
		sleep( SECONDS_TO_MS(5) );
		swritech( 'F' );
	}

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}

/*
** User G sleeps for 15 seconds at a time.
*/

int32_t user_g( void *arg ) {
	// cast away warnings
	(void) arg;


	report( 'G', arg, getpid() );
	swritech( 'G' );
	for( int i = 0; i < 5 ; ++i ) {
		sleep( SECONDS_TO_MS(15) );
		swritech( 'G' );
	}

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}


/*
** User H is like A-C except it only loops 5 times and doesn't
** call exit().
*/

int32_t user_h( void *arg ) {
	// cast away warnings
	(void) arg;


	swritech( 'H' );
	for( int i = 0; i < 5; ++i ) {
		for( int j = 0; j < DELAY_STD; ++j )
			continue;
		swritech( 'H' );
	}

	cwrites( "User H returning without exiting!\n" );

	return( 0 );

}


/*
** User J tries to spawn 2*N_PROCS copies of user_y.
*/

int32_t user_j( void *arg ) {
	// cast away warnings
	(void) arg;


	swritech( 'J' );

	for( int i = 0; i < N_PROCS * 2 ; ++i ) {
		pid_t pid = spawn( user_y, (void *) i, P_HIGH );
		if( pid < 0 ) {
			swritech( 'j' );
		} else {
			swritech( 'J' );
		}
	}

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}

/*
** User K prints, goes into a loop which runs three times, and exits.
** In the loop, it does a spawn of user_x, sleeps 30 seconds, and prints.
*/

int32_t user_k( void *arg ) {
	// cast away warnings
	(void) arg;


	swritech( 'K' );

	for( int i = 0; i < 3 ; ++i ) {
		swritech( 'K' );
		pid_t pid = spawn( user_x, (void *) i, P_HIGH );
		if( pid < 0 ) {
			swritech( 'k' );
		}
		sleep( SECONDS_TO_MS(30) );
	}

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** User L is like user K, except that it prints times and doesn't sleep
** each time, it just preempts itself.
*/

int32_t user_l( void *arg ) {
	// cast away warnings
	(void) arg;

	int i;
	time_t time;
	char buf[8];

	time = gettime();
	cwrites( "User L running, initial time " );
	i = cvt_hex( buf, time );
	cwrite( buf, i );
	cwritech( '\n' );

	swritech( 'L' );

	for( i = 0; i < 3 ; ++i ) {
		swritech( 'L' );
		pid_t pid = spawn( user_x, (void *) i, P_HIGH );
		if( pid < 0 ) {
			swritech( 'l' );
		} else {
			// yield, but don't sleep
			sleep( 0 );
		}
	}

	time = gettime();
	cwrites( "User L exiting, time " );
	i = cvt_hex( buf, time );
	cwrite( buf, i );
	cwritech( '\n' );

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** User M iterates spawns three copies of user W,
** reporting their PIDs.
*/

int32_t user_m( void *arg ) {
	// cast away warnings
	(void) arg;

	int i;
	char buf[12];

	swritech( 'M' );

	for( i = 0; i < 3; ++i ) {
		swritech( 'M' );
		pid_t pid = spawn( user_w, 0, P_HIGH );
		if( pid < 0 ) {
			swritech( 'm' );
		} else {
			cwrites( "User M spawned W, PID " );
			i = cvt_dec( buf, pid );
			cwrite( buf, i );
			cwritech( '\n' );
		}
	}

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}


/*
** User N is like user M, except that it spawns user W and user Z
** at low priority
*/

int32_t user_n( void *arg ) {
	// cast away warnings
	(void) arg;

	int i;
	pid_t pid;
	char buf[12];

	swritech( 'N' );

	for( i = 0; i < 3; ++i ) {
		swritech( 'N' );
		pid = spawn( user_w, (void *) i, P_LOW );
		if( pid < 0 ) {
			swritech( 'n' );
		} else {
			cwrites( "User N spawned W, PID " );
			i = cvt_dec( buf, pid );
			cwrite( buf, i );
			cwritech( '\n' );
		}

		pid = spawn( user_z, (void *) i, P_LOW );
		if( pid < 0 ) {
			swritech( 'n' );
		} else {
			cwrites( "User N spawned Z, PID " );
			i = cvt_dec( buf, pid );
			cwrite( buf, i );
			cwritech( '\n' );
		}
	}

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** User P iterates three times.  Each iteration sleeps for two seconds,
** then gets and prints the system time.
*/

int32_t user_p( void *arg ) {
	// cast away warnings
	(void) arg;

	time_t time;
	int i;
	char buf[8];

	cwrites( "User P running, start at " );
	time = gettime();
	i = cvt_hex( buf, time );
	cwrite( buf, i );
	cwritech( '\n' );

	swritech( 'P' );

	for( i = 0; i < 3; ++i ) {
		sleep( SECONDS_TO_MS(2) );
		time = gettime();
		cwrites( "User P reporting time " );
		i = cvt_hex( buf, time );
		cwrite( buf, i );
		cwritech( '\n' );
		swritech( 'P' );
	}

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** User Q does a bogus system call
*/

int32_t user_q( void *arg ) {
	// cast away warnings
	(void) arg;


	swritech( 'Q' );
	bogus();
	cwrites( "User Q returned from bogus syscall!?!?!\n" );
	exit( EXIT_FAILURE );

	return( 0 );  // shut the compiler up!

}


/*
** User R loops 3 times reading/writing, then exits.
*/

int32_t user_r( void *arg ) {
	// cast away warnings
	(void) arg;

	int ch = '&';
	char buf[12];

	swritech( 'R' );
	sleep( SECONDS_TO_MS(10) );
	for( int i = 0; i < 3; ++i ) {
		do {
			swritech( 'R' );
			ch = sreadch();
			if( ch < 0 ) {
				cwrites( "User R, read returned " );
				int j = cvt_dec( buf, ch );
				cwrite( buf, j );
				cwritech( '\n' );
				if( ch == -1 ) {
					// wait a bit
					sleep( SECONDS_TO_MS(1) );
				}
			}
		} while( ch < 0 );
		swritech( ch );
	}

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** User S sleeps for 20 seconds at a time, and loops forever.
*/

int32_t user_s( void *arg ) {
	// cast away warnings
	(void) arg;


	swritech( 'S' );
	for(;;) {
		sleep( SECONDS_TO_MS(20) );
		swritech( 'S' );
	}

	cwrites( "User S exiting!?!?!n" );
	swritech( 's' );
	exit( EXIT_FAILURE );

	return( 0 );  // shut the compiler up!

}

/*
** User T iterates three times, spawning copies of user W; then it
** sleeps for eight seconds, and then waits for those processes.
*/

int32_t user_t( void *arg ) {
	// cast away warnings
	(void) arg;

	pid_t pid[3];
	char buf[12];

	swrite( "T", 1 );

	for( int i = 0; i < 3; ++i ) {
		pid[i] = spawn( user_w, (void *) i, P_LOW );
		if( pid[i] < 0 ) {
			swritech( 't' );
		} else {
			swrite( "T", 1 );
		}
	}

	sleep( SECONDS_TO_MS(8) );

	// collect exit status information

	do {
		pid_t pid;
		int32_t exitstatus;
		status_t status = wait( &pid, &exitstatus );
		if( status == E_NO_CHILD ) {
			break;
		} else if( status != E_SUCCESS ) {
			prt_status( "User T: wait() status ", status );
			break;
		}
		cwrites( "User T: child PID " );
		int j = cvt_dec( buf, pid );
		cwrite( buf, j );
		prt_status( " exit status ", exitstatus );
	} while( 1 );

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}


/*
** User U is like user T, except that it kills any living children
** after sleeping.
*/

int32_t user_u( void *arg ) {
	// cast away warnings
	(void) arg;

	int pid[3];
	status_t status;
	char buf[12];


	for( int i = 0; i < 3; ++i ) {
		pid[i] = spawn( user_w, (void *) i, P_HIGH );
		if( pid[i] < 0 ) {
			swritech( 'u' );
		} else {
			cwrites( "User U spawned W, PID " );
			int j = cvt_dec( buf, pid[i] );
			cwrite( buf, j );
			cwritech( '\n' );
			swrite( "U", 1 );
		}
	}

	sleep( SECONDS_TO_MS(8) );

	for( int i = 0; i < 3; ++i ) {
		if( pid[i] > 0 ) {
			status = kill( pid[i] );
			cwrites( "User U kill PID " );
			int j = cvt_dec( buf, pid[i] );
			cwrite( buf, j );
			cwrites( " status " );
			j = cvt_dec( buf, status );
			cwrite( buf, j );
			cwritech( '\n' );
		}
	}

	// collect exit status information

	do {
		pid_t pid;
		int32_t exitstatus;
		status = wait( &pid, &exitstatus );
		if( status == E_NO_CHILD ) {
			break;
		} else if( status != E_SUCCESS ) {
			prt_status( "User U: wait() status ", status );
			break;
		}
		cwrites( "User U: child " );
		int j = cvt_dec( buf, pid );
		cwrite( buf, j );
		cwrites( " exit status " );
		j = cvt_dec( buf, status );
		cwrite( buf, j );
		cwritech( '\n' );
	} while( 1 );

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}


/*
** User W prints W characters 20 times, sleeping 3 seconds between.
*/

int32_t user_w( void *arg ) {
	// cast away warnings
	(void) arg;

	char buf[12];

	report( 'W', arg, getpid() );

	for( int i = 0; i < 20 ; ++i ) {
		swritech( 'W' );
		sleep( SECONDS_TO_MS(3) );
	}

	cwrites( "User W (" );
	int j = cvt_dec( buf, (int) arg );
	cwrite( buf, j );
	cwrites( ") exiting\n" );

	exit( EXIT_SUCCESS );
	return( 0 );  // shut the compiler up!

}


/*
** User X prints X characters 20 times.  It is spawned multiple
** times, prints its PID when started and exiting, and exits
** with a non-zero status
*/

int32_t user_x( void *arg ) {
	// cast away warnings
	(void) arg;

	int i, j;
	pid_t pid;
	char buf[12];

	report( 'X', arg, pid=getpid() );

	for( i = 0; i < 20 ; ++i ) {
		swritech( 'X' );
		for( j = 0; j < DELAY_STD; ++j )
			continue;
	}

	cwrites( "User X (" );
	i = cvt_dec( buf, (int) arg );
	cwrite( buf, i );
	cwrites( ") exiting\n" );
	exit( pid ? pid : -98765 );

	return( 0 );  // shut the compiler up!

}


/*
** User Y prints Y characters 10 times.
*/

int32_t user_y( void *arg ) {
	// cast away warnings
	(void) arg;

	int i, j;

	for( i = 0; i < 10 ; ++i ) {
		swritech( 'Y' );
		for( j = 0; j < DELAY_ALT; ++j )
			continue;
		sleep( SECONDS_TO_MS(1) );
	}

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** User Z prints Z characters 10 times.  Before it exits, its parent
** may have exited; it reports this so that we can verify reparenting.
*/

int32_t user_z( void *arg ) {
	// cast away warnings
	(void) arg;

	int i, j;
	pid_t pid, ppid;
	char buf[12];

	pid = getpid();
	ppid = getppid();

	cwrites( "User Z (" );
	i = cvt_dec( buf, pid );
	cwrite( buf, i );
	cwrites( ") running, parent " );
	i = cvt_dec( buf, ppid );
	cwrite( buf, i );
	cwritech( '\n' );

	for( i = 0; i < 10 ; ++i ) {
		swritech( 'Z' );
		for( j = 0; j < DELAY_STD; ++j )
			continue;
	}

	// get "new" parent PID
	ppid = getppid();

	cwrites( "User Z (" );
	i = cvt_dec( buf, pid );
	cwrite( buf, i );
	cwrites( ") exiting, parent now " );
	i = cvt_dec( buf, ppid );
	cwrite( buf, i );
	cwritech( '\n' );

	exit( EXIT_SUCCESS );

	return( 0 );  // shut the compiler up!

}


/*
** SYSTEM PROCESSES
*/

/*
** Idle process
**
** configurable - uses the argument as the character to print
*/

int32_t idle( void *arg ) {
	// cast away warnings
	(void) arg;

	int i;
	pid_t pid;
	time_t time;
	char buf[12];
	char ch = (int32_t) arg;

	pid = getpid();
	time = gettime();
	cwrites( "Idle (" );
	i = cvt_dec( buf, pid );
	cwrite( buf, i );
	cwrites( ") started @ " );
	i = cvt_hex( buf, time );
	cwrite( buf, i );
	cwritech( '\n' );

	swritech( ch );

	for(;;) {
		for( i = 0; i < DELAY_LONG; ++i )
			continue;
		swritech( ch );
	}

	time = gettime();
	cwrites( "+++ Idle done @ " );
	i = cvt_hex( buf, time );
	cwrite( buf, i );
	cwrites( "!?!?!\n" );

	exit( EXIT_FAILURE );
	return( 0 );  // shut the compiler up!

}

int32_t redrawProcess(void *arg) {

	

	for (;;) {
		if (CIO_CONTROLLER.dirty) {
			vcon_redraw(&CIO_CONTROLLER);
			CIO_CONTROLLER.dirty = 0;
		}
		sleep(10);
	}

	return 0;
}
/*
** Initial process; it starts the other top-level user processes.
**
** Prints '$' at startup, '+' after each user process is spawned,
** and '!' before transitioning to wait() mode to the SIO, and
** startup and transition messages to the console.  It also reports
** each child process it collects via wait() to the console along
** with that child's exit status.
*/

int32_t init( void *arg ) {
	// cast away warnings
	(void) arg;
	pid_t pid;

	cwrites( "Init started\n" );

	swritech( '$' );

	// pid = spawn( idle, (void *) '.', P_MIN );
	pid = spawn( idle, 0, P_MIN );
	if( pid < 0 ) {
		cwrites( "init, spawn() IDLE failed\n" );
	}
	swritech( '+' );

	// startup network daemons
	spawn(nic_rx_daemon, 0, P_SYSTEM);
	spawn(nic_tx_daemon, 0, P_SYSTEM);

	// Launch test program for networking
	pid = spawn(net_test_main, 0, P_SYSTEM);
	if(pid < 0) {
		cwrites("init, spawn() net_test_main failed\n");
	}
	//swritech('{');
	spawn(redrawProcess, 0, P_SYSTEM);

#ifdef SPAWN_A
	pid = spawn( user_a, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user A failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_B
	pid = spawn( user_b, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user B failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_C
	pid = spawn( user_c, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user C failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_D
	pid = spawn( user_d, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user D failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_E
	pid = spawn( user_e, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user E failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_F
	pid = spawn( user_f, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user F failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_G
	pid = spawn( user_g, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user G failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_H
	pid = spawn( user_h, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user H failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_J
	pid = spawn( user_j, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user J failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_K
	pid = spawn( user_k, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user K failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_L
	pid = spawn( user_l, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user L failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_M
	pid = spawn( user_m, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user M failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_N
	pid = spawn( user_n, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user N failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_P
	pid = spawn( user_p, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user P failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_Q
	pid = spawn( user_q, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user Q failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_R
	pid = spawn( user_r, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user R failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_S
	pid = spawn( user_s, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user S failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_T
	pid = spawn( user_t, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user T failed\n" );
	}
	swritech( '+' );
#endif

#ifdef SPAWN_U
	pid = spawn( user_u, 0, P_HIGH );
	if( pid < 0 ) {
		cwrites( "init, spawn() user U failed\n" );
	}
	swritech( '+' );
#endif

	swritech( '!' );
	cwrites( "!\n" );

	/*
	** At this point, we go into an infinite loop waiting
	** for our children (direct, or inherited) to exit.
	*/

	cwrites( "init() transitioning to wait() mode\n" );

	for(;;) {
		int i;
		pid_t pid;
		int32_t exitstatus;
		char buf[12];
		status_t status = wait( &pid, &exitstatus );
		if( status == E_SUCCESS ) {
			cwrites( "INIT: pid " );
			i = cvt_dec( buf, pid );
			cwrite( buf, i );
			cwrites( " exited, status " );
			i = cvt_dec( buf, status );
			cwrite( buf, i );
			cwritech( '\n' );
		} else {
			prt_status( "*** init(): wait() returned status",
				    status );
		}
	}

	/*
	** SHOULD NEVER REACH HERE
	*/

	cwrites( "*** INIT IS EXITING???\n" );
	exit( EXIT_FAILURE );

	return( 0 );  // shut the compiler up!
}
