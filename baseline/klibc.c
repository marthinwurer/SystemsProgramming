/*
** SCCS ID:	@(#)klibc.c	1.1	4/13/17
**
** File:	klibc.c
**
** Author:	Warren R. Carithers and various CSCI-452 sections
**
** Contributor:
**
** Description:	C implementations of kernel library functions
*/

#define	__SP_KERNEL__

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
** _put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/
void _put_char_or_code( int ch ) {

	if( ch >= ' ' && ch < 0x7f ) {
		c_putchar( ch );
	} else {
		c_printf( "\\x%02x", ch );
	}
}

/*
** _kmemset - set all bytes of a block of memory to a specific value
**
** usage:  _kmemset( buffer, length, value )
*/
void _kmemset( register uint8_t *buf, register uint32_t len, register uint8_t value ) {

	while( len-- ) {
		*buf++ = value;
	}

}

/*
** _kmemclr - initialize all bytes of a block of memory to zero
**
** usage:  _kmemclr( buffer, length )
*/
void _kmemclr( register uint8_t *buf, register uint32_t len ) {

	while( len-- ) {
		*buf++ = 0;
	}

}

/*
** _kmemcpy - copy a block from one place to another
**
** usage:  _kmemcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/
void _kmemcpy( register uint8_t *dst, register uint8_t *src, register uint32_t len ) {

	while( len-- ) {
		*dst++ = *src++;
	}

}

/*
** _kstrcmp - compare two NUL-terminated strings
*/
int _kstrcmp( register const char *s1, register const char *s2 ) {

	while( *s1 != 0 && (*s1 == *s2) )
		++s1, ++s2;

	return( *(const unsigned char *)s1 - *(const unsigned char *)s2 );
}

/*
** _kpanic - kernel-level panic routine
**
** usage:  _kpanic( mod, msg )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
**
** 'mod' argument is always printed; 'msg' argument is printed
** if it isn't NULL, followed by a newline
*/
void _kpanic( char *mod, char *msg ) {

	c_puts( "\n\n***** KERNEL PANIC *****\n\n" );
	c_printf( "Module:  %s\n", mod );
	if( msg != NULL ) {
		c_printf( "Message: %s\n", msg );
	}

	//
	// This might be a good place to do a stack frame
	// traceback
	//

	__panic( "KERNEL PANIC" );
}
