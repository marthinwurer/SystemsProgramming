/*
** SCCS ID:	@(#)ulib.h	1.1	4/13/17
**
** File:	ulib.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	declarations for user-level library functions
*/

#ifndef _ULIB_H_
#define _ULIB_H_

#include "common.h"

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

/*
** Globals
*/

/*
** Prototypes
*/

/*
**********************************************
** CONVENIENT "SHORTHAND" VERSIONS OF SYSCALLS
**********************************************
*/

/*
** creadch - read one character from the console
**
** usage:	n = creadch();
**
** reads one character from the console and return it
**
** returns:
**      the character read, or an error code
*/
int32_t creadch( void );

/*
** cread - read into a buffer from the console
**
** usage:	n = cread(buf,size);
**
** reads up to 'size' characters from the console into 'buf'
**
** returns:
**      the count of characters read, or an error code
*/
int32_t cread( char *buf, uint32_t size );

/*
** sreadch - read one character from the SIO
**
** usage:	n = sreadch();
**
** reads one character from the SIO and return it
**
** returns:
**      the character read, or an error code
*/
int32_t sreadch( void );

/*
** sread - read into a buffer from the SIO
**
** usage:	n = sread(buf,size);
**
** reads up to 'size' characters from the SIO into 'buf'
**
** returns:
**      the count of characters read, or an error code
*/
int32_t sread( char *buf, uint32_t size );

/*
** cwrites - write a NUL-terminated string to the console
**
** usage:	n = cwrites(str);
**
** writes a NUL-terminate string to the console
**
** returns:
**      the number of characters written, or an error code
*/
int32_t cwrites( const char *str );

/*
** cwritech - write one character to the console
**
** usage:	n = cwritech(ch);
**
** writes one character to the console
**
** returns:
**      the number of characters written, or an error code
*/
int32_t cwritech( char ch );

/*
** cwrite - write from a buffer to the console
**
** usage:	n = cread(buf,size);
**
** writes up to 'size' characters from 'buf' to the console
**
** returns:
**      the count of characters written, or an error code
*/
int32_t cwrite( const char *buf, uint32_t size );

/*
** swritech - write one character to the SIO
**
** usage:	n = swritech(ch);
**
** writes one character to the SIO and return it
**
** returns:
**      the number of characters written, or an error code
*/
int32_t swritech( char ch );

/*
** swrites - write a NUL-terminated string to the SIO
**
** usage:	n = swrites(str);
**
** writes a NUL-terminate string to the SIO
**
** returns:
**      the number of characters written, or an error code
*/
int32_t swrites( const char *str );

/*
** swrite - write from a buffer to the SIO
**
** usage:	n = swrite(buf,size);
**
** writes up to 'size' characters from 'buf' to the SIO
**
** returns:
**      the count of characters written, or an error code
*/
int32_t swrite( const char *buf, uint32_t size );

/*
** spawn - create a new process running a different program
**
** usage:	n = spawn(entry,arg,prio)
**
** performs a fork() followed by an exec(entry,arg,prio) by the child
**
** returns:
**	the PID of the child, or an error code
*/
pid_t spawn(  int (*entry)(void *), void *arg, prio_t prio );

/*
**********************************************
** SYSTEM CALL STUBS
**********************************************
*/

/*
** exit - terminate the calling process
**
** usage:	exit(status);
**
** does not return
*/
void exit( uint32_t status );

/*
** read - read into a buffer from a channel
**
** usage:	n = read(chan,buf,size)
**
** returns:
**	count of characters transferred, or an error code
*/
int32_t read( uint32_t chan, char *buf, uint32_t size );

/*
** write - write from a buffer to a channel
**
** usage:	n = write(chan,buf,size)
**
** returns:
**	count of characters transferred, or an error code
*/
int32_t write( uint32_t chan, const char *buf, uint32_t size );

/*
** fork - create a new process
**
** usage:	pid = fork();
**
** returns:
**	parent - pid of the spawned process, or -1 on failure
**	child - 0
*/
int32_t fork( void );

/*
** exec - replace this program with a different one
**
** usage:	pid = exec(entry,arg,prio);
**
** returns:
**	on success, does not return
**	on failure, an error code
*/
int32_t exec( int32_t (*entry)(void *), void *arg, prio_t prio );

/*
** kill - terminate a process with extreme prejudice
**
** usage:       status = kill(pid);
**
** returns:
**      status of the termination attempt
*/
status_t kill( pid_t pid );

/*
** wait - wait for a child process to terminate
**
** usage:	status = wait(pid,status);
**
** if there are no children in the system, returns an error code
**
** if there are one or more children in the system and at least one has
** terminated but hasn't yet been cleaned up, cleans up that process and
** returns its information; otherwise, blocks until a child terminates
**
** returns:
**      on error, an error code
**      on success, E_SUCCESS, and the pid and exit status of the child
*/
status_t wait( pid_t *pid, int32_t *status );

/*
** sleep - put the current process to sleep for some length of time
**
** usage:	sleep(n);
**
** puts the process onto the sleep queue for the specified
** number of milliseconds
*/
void sleep( uint32_t msec );

/*
** getpid - retrieve PID of this process
**
** usage:	n = getpid();
**
** returns:
**      the PID of this process
*/
pid_t getpid( void );

/*
** getppid - retrieve PID of the parent of this process
**
** usage:	n = getppid();
**
** returns:
**      the PID of the parent of this process
*/
pid_t getppid( void );

/*
** gettime - retrieve the current system time
**
** usage:	n = gettime();
**
** returns:
**      the current system time
*/
time_t gettime( void );

/*
** getstatus - retrieve the state of the specified process
**
** usage:	state = getstatus(pid);
**
** returns:
**      the state of the process, or an error code
*/
state_t getstatus( pid_t pid );

/*
** bogus - a bogus system call, for testing our syscall ISR
**
** usage:	bogus();
*/
void bogus( void );

/*
**********************************************
** MISCELLANEOUS USEFUL SUPPORT FUNCTIONS
**********************************************
*/

/*
** strlen()
**
** return length of a NUL-terminated string
*/
uint32_t strlen( const char *msg );

/*
** cvt_dec()
**
** convert a 32-bit integer value into a
** NUL-terminated character string
**
** returns the number of characters placed into the buffer
** (not including the NUL)
*/
int cvt_dec( char *buf, int32_t value );

/*
** cvt_hex()
**
** convert a 32-bit value into an (up to) 8-character
** NUL-terminated character string
**
** returns the number of characters placed into the buffer
** (not including the NUL)
*/
int cvt_hex( char *buf, uint32_t value );

/*
** do_exit()
**
** support function used as the "return to" address
** for user main() routines; it just calls exit(%eax)
*/
void do_exit( void );

/*
** prt_status - print a status return value to the console
**
** the "msg" argument is printed, followed by a hex representation
** of the "stat" argument
*/
void prt_status( char *msg, status_t stat );

#endif

#endif
