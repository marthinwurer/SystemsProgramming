/*
** SCCS ID:	@(#)c_io.c	1.13	03/08/10
**
** File:	c_io.c
**
** Author:	K. Reek
**
** Contributor:	Jon Coles, Warren R. Carithers
**
** Description:	Console I/O routines
**
**	This module implements a simple set of input and output routines
**	for the console screen and keyboard on the Soyatas in the DSL.
**	Refer to the printed documentation for complete details.
**
*/

#include <baseline/c_io.h>
#include <baseline/startup.h>
#include <baseline/support.h>
#include <x86arch.h>

#include <kern/vconsole/buffer.h>

/*
** Video parameters, and state variables
*/
#define	SCREEN_MIN_X	0
#define	SCREEN_MIN_Y	0
#define	SCREEN_X_SIZE	80
#define	SCREEN_Y_SIZE	25
#define	SCREEN_MAX_X	( SCREEN_X_SIZE - 1 )
#define	SCREEN_MAX_Y	( SCREEN_Y_SIZE - 1 )



#define	VIDEO_ADDR(x,y)	( unsigned short * ) \
		( VIDEO_BASE_ADDR + 2 * ( (y) * SCREEN_X_SIZE + (x) ) )

#include <kern/vconsole/console.h>
#include <kern/vconsole/control.h>

static VConLine *TEMP_CONSOLE_LINETAB = (VConLine*)0x4C00;

static VConChar *TEMP_CONSOLE_CHARTAB = (VConChar*)0x4D40;

static VCon CON;

int CIO_AUTOFLUSH;


VConCtrl CIO_CONTROLLER = { .mode = VCON_MODE_VGATEXT };


static unsigned int __c_strlen( char const *str ){
	unsigned int	len = 0;

	while( *str++ != '\0' ){
		len += 1;
	}
	return len;
}

void c_flush(void) {
	vcon_redraw(&CIO_CONTROLLER);
}


void c_setscroll( unsigned int s_min_x, unsigned int s_min_y, unsigned int s_max_x, unsigned int s_max_y ){
	(void)s_min_x; (void)s_max_x;
	vcon_setScroll(&CON, s_min_y, s_max_y);
}

/*
** Cursor movement in the scroll region
*/
void c_moveto( unsigned int x, unsigned int y ){
	vcon_setCursor(&CON, x, y);
}

/*
** The putchar family
*/
void c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
	vcon_putcharAt(&CON, c, x, y);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}

#ifndef SA_DEBUG
void c_putchar( unsigned int c ){
	vcon_putchar(&CON, c);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}
#endif

/*
** The puts family
*/
void c_puts_at( unsigned int x, unsigned int y, char *str ){
	vcon_putsAt(&CON, str, x, y);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}

#ifndef SA_DEBUG
void c_puts( char *str ){
	vcon_puts(&CON, str);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}
#endif

void c_clearscroll( void ){
	vcon_clearScroll(&CON);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}

void c_clearscreen( void ){
	vcon_clear(&CON);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}


void c_scroll( unsigned int lines ){
	vcon_scroll(&CON, lines);
	if (CIO_AUTOFLUSH)
		vcon_redraw(&CIO_CONTROLLER);
}

char * cvtdec0( char *buf, int value ){
	int	quotient;

	quotient = value / 10;
	if( quotient < 0 ){
		quotient = 214748364;
		value = 8;
	}
	if( quotient != 0 ){
		buf = cvtdec0( buf, quotient );
	}
	*buf++ = value % 10 + '0';
	return buf;
}

int cvtdec( char *buf, int value ){
	char	*bp = buf;

	if( value < 0 ){
		*bp++ = '-';
		value = -value;
	}
	bp = cvtdec0( bp, value );
	*bp = '\0';

	return bp - buf;
}

char hexdigits[] = "0123456789ABCDEF";

int cvthex( char *buf, int value ){
	int	i;
	int	chars_stored = 0;
	char	*bp = buf;

	for( i = 0; i < 8; i += 1 ){
		int	val;

		val = ( value & 0xf0000000 );
		if( i == 7 || val != 0 || chars_stored ){
			chars_stored = 1;
			val >>= 28;
			val &= 0xf;
			*bp++ = hexdigits[ val ];
		}
		value <<= 4;
	}
	*bp = '\0';

	return bp - buf;
}

int cvtoct( char *buf, int value ){
	int	i;
	int	chars_stored = 0;
	char	*bp = buf;
	int	val;

	val = ( value & 0xc0000000 );
	val >>= 30;
	for( i = 0; i < 11; i += 1 ){

		if( i == 10 || val != 0 || chars_stored ){
			chars_stored = 1;
			val &= 0x7;
			*bp++ = hexdigits[ val ];
		}
		value <<= 3;
		val = ( value & 0xe0000000 );
		val >>= 29;
	}
	*bp = '\0';

	return bp - buf;
}

static int pad( int x, int y, int extra, int padchar ){
	while( extra > 0 ){
		if( x != -1 || y != -1 ){
			c_putchar_at( x, y, padchar );
			x += 1;
		}
		else {
			c_putchar( padchar );
		}
		extra -= 1;
	}
	return x;
}

static int padstr( int x, int y, char *str, int len, int width, int leftadjust, int padchar ){
	int	extra;

	if( len < 0 ){
		len = __c_strlen( str );
	}
	extra = width - len;
	if( extra > 0 && !leftadjust ){
		x = pad( x, y, extra, padchar );
	}
	if( x != -1 || y != -1 ){
		c_puts_at( x, y, str );
		x += len;
	}
	else {
		c_puts( str );
	}
	if( extra > 0 && leftadjust ){
		x = pad( x, y, extra, padchar );
	}
	return x;
}

static void __c_do_printf( int x, int y, char **f ){
	char	*fmt = *f;
	int	*ap;
	char	buf[ 12 ];
	char	ch;
	char	*str;
	int	leftadjust;
	int	width;
	int	len;
	int	padchar;

	/*
	** Get characters from the format string and process them
	*/
	ap = (int *)( f + 1 );
	while( (ch = *fmt++) != '\0' ){
		/*
		** Is it the start of a format code?
		*/
		if( ch == '%' ){
			/*
			** Yes, get the padding and width options (if there).
			** Alignment must come at the beginning, then fill,
			** then width.
			*/
			leftadjust = 0;
			padchar = ' ';
			width = 0;
			ch = *fmt++;
			if( ch == '-' ){
				leftadjust = 1;
				ch = *fmt++;
			}
			if( ch == '0' ){
				padchar = '0';
				ch = *fmt++;
			}
			while( ch >= '0' && ch <= '9' ){
				width *= 10;
				width += ch - '0';
				ch = *fmt++;
			}

			/*
			** What data type do we have?
			*/
			switch( ch ){
			case 'c':
				// ch = *( (int *)ap )++;
				ch = *ap++;
				buf[ 0 ] = ch;
				buf[ 1 ] = '\0';
				x = padstr( x, y, buf, 1, width, leftadjust, padchar );
				break;

			case 'd':
				// len = cvtdec( buf, *( (int *)ap )++ );
				len = cvtdec( buf, *ap++ );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 's':
				// str = *( (char **)ap )++;
				str = (char *) (*ap++);
				x = padstr( x, y, str, -1, width, leftadjust, padchar );
				break;

			case 'x':
				// len = cvthex( buf, *( (int *)ap )++ );
				len = cvthex( buf, *ap++ );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 'o':
				// len = cvtoct( buf, *( (int *)ap )++ );
				len = cvtoct( buf, *ap++ );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			}
		}
		else {
			if( x != -1 || y != -1 ){
				c_putchar_at( x, y, ch );
				switch( ch ){
				case '\n':
					y += 1;
					/* FALL THRU */

				case '\r':
					x = 0;
					break;

				default:
					x += 1;
				}
			}
			else {
				c_putchar( ch );
			}
		}
	}
}

void c_printf_at( unsigned int x, unsigned int y, char *fmt, ... ){
	__c_do_printf( x, y, &fmt );
}

void c_printf( char *fmt, ... ){
	__c_do_printf( -1, -1, &fmt );
}

unsigned char scan_code[ 2 ][ 128 ] = {
	{
/* 00-07 */	'\377',	'\033',	'1',	'2',	'3',	'4',	'5',	'6',
/* 08-0f */	'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
/* 10-17 */	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
/* 18-1f */	'o',	'p',	'[',	']',	'\n',	'\377',	'a',	's',
/* 20-27 */	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',
/* 28-2f */	'\'',	'`',	'\377',	'\\',	'z',	'x',	'c',	'v',
/* 30-37 */	'b',	'n',	'm',	',',	'.',	'/',	'\377',	'*',
/* 38-3f */	'\377',	' ',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 40-47 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'7',
/* 48-4f */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50-57 */	'2',	'3',	'0',	'.',	'\377',	'\377',	'\377',	'\377',
/* 58-5f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 60-67 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 68-6f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 70-77 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 78-7f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377'
	},

	{
/* 00-07 */	'\377',	'\033',	'!',	'@',	'#',	'$',	'%',	'^',
/* 08-0f */	'&',	'*',	'(',	')',	'_',	'+',	'\b',	'\t',
/* 10-17 */	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
/* 18-1f */	'O',	'P',	'{',	'}',	'\n',	'\377',	'A',	'S',
/* 20-27 */	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
/* 28-2f */	'"',	'~',	'\377',	'|',	'Z',	'X',	'C',	'V',
/* 30-37 */	'B',	'N',	'M',	'<',	'>',	'?',	'\377',	'*',
/* 38-3f */	'\377',	' ',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 40-47 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'7',
/* 48-4f */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50-57 */	'2',	'3',	'0',	'.',	'\377',	'\377',	'\377',	'\377',
/* 58-5f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 60-67 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 68-6f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 70-77 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 78-7f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377'
	}
};

#define	C_BUFSIZE	200
#define	KEYBOARD_DATA	0x60
#define	KEYBOARD_STATUS	0x64
#define	READY		0x1
#define	EOT		'\04'

/*
** Circular buffer for input characters.  Characters are inserted at
** __c_next_space, and are removed at __c_next_char.  Buffer is empty if
** these are equal.
*/
static	char	__c_input_buffer[ C_BUFSIZE ];
static	volatile char	*__c_next_char = __c_input_buffer;
static	volatile char	*__c_next_space = __c_input_buffer;

static	volatile char *__c_increment( volatile char *pointer ){
	if( ++pointer >= __c_input_buffer + C_BUFSIZE ){
		pointer = __c_input_buffer;
	}
	return pointer;
}

static void __c_input_scan_code( int code ){
	static	int	shift = 0;
	static	int	ctrl_mask = 0xff;

	/*
	** Do the shift processing
	*/
	code &= 0xff;
	switch( code ){
	case 0x2a:
	case 0x36:
		shift = 1;
		break;

	case 0xaa:
	case 0xb6:
		shift = 0;
		break;

	case 0x1d:
		ctrl_mask = 0x1f;
		break;

	case 0x9d:
		ctrl_mask = 0xff;
		break;

	default:
		/*
		** Process ordinary characters only on the press
		** (to handle autorepeat).
		** Ignore undefined scan codes.
		*/
		if( ( code & 0x80 ) == 0 ){
			code = scan_code[ shift ][ (int)code ];
			if( code != '\377' ){
				volatile char	*next = __c_increment( __c_next_space );

				/*
				** Store character only if there's room
				*/
				if( next != __c_next_char ){
					*__c_next_space = code & ctrl_mask;
					__c_next_space = next;
				}
			}
		}
	}
}

static void __c_keyboard_isr( int vector, int code ){
	__c_input_scan_code( __inb( KEYBOARD_DATA ) );
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
	(void) vector;
	(void) code;
}

int c_getchar( void ){
	char	c;
	int	interrupts_enabled = __get_flags() & EFLAGS_IF;

	while( __c_next_char == __c_next_space ){
		if( !interrupts_enabled ){
			/*
			** Must read the next keystroke ourselves.
			*/
			while( ( __inb( KEYBOARD_STATUS ) & READY ) == 0 ){
				;
			}
			__c_input_scan_code( __inb( KEYBOARD_DATA ) );
		}
	}

	c = *__c_next_char & 0xff;
	__c_next_char = __c_increment( __c_next_char );
	if( c != EOT ){
		c_putchar( c );
	}
	return c;
}

int c_gets( char *buffer, unsigned int size ){
	char	ch;
	int	count = 0;

	while( size > 1 ){
		ch = c_getchar();
		if( ch == EOT ){
			break;
		}
		*buffer++ = ch;
		count += 1;
		size -= 1;
		if( ch == '\n' ){
			break;
		}
	}
	*buffer = '\0';
	return count;
}

int c_input_queue( void ){
	int	n_chars = __c_next_space - __c_next_char;

	if( n_chars < 0 ){
		n_chars += C_BUFSIZE;
	}
	return n_chars;
}

/*
** Initialization routines
*/
void c_io_init( void ){
	
	CIO_CONTROLLER.current = &CON;

	vcon_init(&CON, SCREEN_Y_SIZE, SCREEN_X_SIZE);
	CON.buf.lineTable = TEMP_CONSOLE_LINETAB;
	CON.buf.charTable = TEMP_CONSOLE_CHARTAB;
	vcon_buf_initLineTable(&CON);

	CON.controller = &CIO_CONTROLLER;
	vcon_clear(&CON);
	vcon_redraw(&CIO_CONTROLLER);

	CIO_AUTOFLUSH = 1;

}

void c_io_init_isr(void) {
	/*
	** Set up the interrupt handler for the keyboard
	*/
	__install_isr( INT_VEC_KEYBOARD, __c_keyboard_isr );
}

