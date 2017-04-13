/*
** SCCS ID:	@(#)ulibc.c	1.1	4/13/17
**
** File:	ulibc.c
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	C implementations of user-level library functions
*/

#include <baseline/common.h>

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

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** First, some general utility functions
*/

/*
** strlen()
**
** return length of a NUL-terminated string
*/
uint32_t strlen( const char *msg ) {
	uint32_t len = 0;

	while( *msg++ ) {
		++len;
	}

	return( len );
}

/*
** cvt_dec0
**
** local support routine for cvt_dec()
**
** copied from c_io.c cvtdec0() routine
*/
char *cvt_dec0( char *buf, int value ) {
	int quotient;

	quotient = value / 10;
	if( quotient < 0 ) {
		quotient = 214748364;
		value = 8;
	}
	if( quotient != 0 ) {
		buf = cvt_dec0( buf, quotient );
	}
	*buf++ = value % 10 + '0';
	return buf;
}

/*
** cvt_dec
**
** convert a 32-bit signed value into a
** NUL-terminated character string
**
** returns the number of characters placed into the buffer
** (not including the NUL)
**
** copied from c_io.c cvtdec() routine
*/
int cvt_dec( char *buf, int32_t value ) {
	char *bp = buf;

	if( value < 0 ) {
		*bp++ = '-';
		value = -value;
	}

	bp = cvt_dec0( bp, value );
	*bp  = '\0';

	return( bp - buf );
}

/*
** cvt_hex
**
** convert a 32-bit value into an (up to) 8-character
** NUL-terminated character string
**
** returns the number of characters placed into the buffer
** (not including the NUL)
**
** copied from c_io.c cvthex() routine
*/
int cvt_hex( char *buf, uint32_t value ) {
	char hexdigits[] = "0123456789ABCDEF";
	int chars_stored = 0;

	for( int i = 0; i < 8; i += 1 ) {
		int val = value & 0xf0000000;
		if( chars_stored || val != 0 || i == 7 ) {
			++chars_stored;
			val = (val >> 28) & 0xf;
			*buf++ = hexdigits[val];
		}
		value <<= 4;
	}

	*buf = '\0';

	return( chars_stored );
}

/*
** Next, some simpler character- and buffer-oriented i/o
** functions for use by user code.
*/

/*
** creadch()
**
** read a single character from the console
*/
int32_t creadch( void ) {
	char buf;
	return( read(CHAN_CONSOLE,&buf,1) );
}

/*
** cread(buf,size)
**
** read into a buffer from the console
*/
int32_t cread( char *buf, uint32_t size ) {
	return( read(CHAN_CONSOLE,buf,size) );
}

/*
** sreadch()
**
** read a single character from the SIO
*/
int32_t sreadch( void ) {
	char buf;
	return( read(CHAN_SIO,&buf,1) );
}

/*
** sread(buf,size)
**
** read into a buffer from the SIO
*/
int32_t sread( char *buf, uint32_t size ) {
	return( read(CHAN_SIO,buf,size) );
}

/*
** cwritech(ch)
**
** write a single character to the console
*/
int32_t cwritech( char ch ) {
	return( write(CHAN_CONSOLE,&ch,1) );
}

/*
** cwrites(str)
**
** write a NUL-terminated string to the console
*/
int32_t cwrites( const char *str ) {
	int len = strlen(str);
	return( write(CHAN_CONSOLE,str,len) );
}

/*
** cwrite(buf,size)
**
** write a sized buffer to the console
*/
int32_t cwrite( const char *buf, uint32_t size ) {
	return( write(CHAN_CONSOLE,buf,size) );
}

/*
** swritech(ch)
**
** write a single character to the SIO
*/
int32_t swritech( char ch ) {
	return( write(CHAN_SIO,&ch,1) );
}

/*
** swrites(str)
**
** write a NUL-terminated string to the SIO
*/
int32_t swrites( const char *str ) {
	int len = strlen(str);
	return( write(CHAN_SIO,str,len) );
}

/*
** swrite(buf,size)
**
** write a sized buffer to the SIO
*/
int32_t swrite( const char *buf, uint32_t size ) {
	return( write(CHAN_SIO,buf,size) );
}

/*      
** prt_status
**
** Print a status value to the console
**      
** The 'msg' argument is printed, followed by
** a hex representation of the 'stat' argument
*/
void prt_status( char *msg, status_t stat ) {
	char buf[8];

	if( msg == NULL ) {
		return;
	}

	cwrite( msg, strlen(msg) );

	int n = cvt_hex( buf, stat );

	cwrite( buf, n );
	cwritech( '\n' );

}

/*
** spawn(entry,arg,prio)
**
** Perform a fork() followed by an exec(entry,arg,prio) by the child
*/
pid_t spawn( int (*entry)(void *), void *arg, prio_t prio ) {
	pid_t new;

	new = fork();
	if( new != 0 ) {	// error, or we are the parent
		return( new );
	}

	// we are the child!

	int32_t num = exec( entry, arg, prio );

	// oops....

	char buf[32];
	new = getpid();			// get our PID
	cvt_dec( buf, (int32_t) new );	// convert to character
	cwrites( "PID " );		// report the error
	cwrite( buf, strlen(buf) );
	cwrites( " exec() failed, status " );
	cvt_dec( buf, num );
	cwrite( buf, strlen(buf) );
	cwritech( '\n' );

	exit( EXIT_FAILURE );

	// keep the compiler happy!
	return( EXIT_FAILURE );
}
