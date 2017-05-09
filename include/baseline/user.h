/*
** SCCS ID:	@(#)user.h	1.1	4/13/17
**
** File:	user.h
**
** Author:	Warren R. Carithers and various CSCI-452 classes
**
** Contributor:
**
** Description:	control of user-level routines
*/

#ifndef _USER_H_
#define _USER_H_

/*
** General (C and/or assembly) definitions
*/

// delay loop counts

#define	DELAY_LONG	100000000
#define	DELAY_STD	  2500000
#define	DELAY_ALT	  4500000

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** User process controls.
**
** The comment field of these definitions contains a list of the
** system calls this user process uses.
**
** To spawn a specific user process from the initial process,
** uncomment its entry in this list.
*/

//	user	          baseline system calls in use
//		   xt cr sr cw sw fe sp wt kl sl gp gP gt gs
// #define SPAWN_A	//  X  .  .  X  X  .  .  .  .  .  .  .  .  .
// #define SPAWN_B	//  X  .  .  X  X  .  .  .  .  .  .  .  .  .
// #define SPAWN_C	//  X  .  .  X  X  .  .  .  .  .  .  .  .  .
// #define SPAWN_D	//  X  .  .  X  X  .  X  .  .  .  .  .  .  .
// #define SPAWN_E	//  X  .  .  X  X  .  .  .  .  X  X  .  .  .
// #define SPAWN_F	//  X  .  .  X  X  .  .  .  .  X  X  .  .  .
// #define SPAWN_G	//  X  .  .  X  X  .  .  .  .  X  X  .  .  .
// #define SPAWN_H	//  .  .  .  X  X  .  .  .  .  .  .  .  .  .
// 		// user H doesn't call exit()
// // no user I
// #define SPAWN_J	//  X  .  .  .  X  .  X  .  .  .  .  .  .  .
// 		// user J tries to spawn() 2*N_PROCS children
// #define SPAWN_K	//  X  .  .  .  X  .  X  .  .  X  .  .  .  .
// #define SPAWN_L	//  X  .  .  X  X  .  .  .  .  .  .  .  X  .
// #define SPAWN_M	//  X  .  .  X  X  .  X  .  .  .  .  .  .  .
// #define SPAWN_N	//  X  .  .  X  X  .  X  .  .  .  .  .  .  .
// 		// user N spawns W and Z at low priority
// // no user O
// #define SPAWN_P	//  X  .  .  X  X  .  .  .  .  X  .  .  X  .
// #define SPAWN_Q	//  X  .  .  X  X  .  .  .  .  .  .  .  .  .
//  		// user Q makes a bogus system call
// #define SPAWN_R	//  X  .  X  X  X  .  .  .  .  X  .  .  .  .
// #define SPAWN_S	//  .  .  X  X  .  .  .  .  .  X  .  .  .  .
// 		// user S loops forever
// #define SPAWN_T	//  X  .  X  X  .  .  .  X  .  X  .  .  .  .
// #define SPAWN_U	//  X  .  X  X  .  .  .  X  X  X  .  .  .  .
// no user V

/*
** Users W-Z are spawned from other processes; they
** should never be spawned directly by init().
*/

// user W:      //  X  .  .  X  X  .  .  .  .  X  X  .  .  .
// user X:      //  X  .  .  X  X  .  .  .  .  .  X  .  .  .
		// user X exits with a non-zero status
// user Y:      //  X  .  .  .  X  .  .  .  .  X  .  .  .  .
// user Z:      //  X  .  .  X  X  .  .  .  .  .  X  X  .  .


/*
** Prototypes for externally-visible routines
*/

/*
** init - initial user process
**
** after spawning the other user processes, loops forever calling wait()
*/

int32_t init( void *arg );

/*
** idle - what to run when there's nothing else to run
*/

int32_t idle( void *arg );

#endif

#endif
