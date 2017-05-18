/*
** File: kern/early/realmode.S
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Implementation for the int32 support function
** Actual implementation of int32 is located in realprog/realprog.S
**
*/

#include <baseline/bootstrap.h>

	.arch i386

	#.global int32, init_int32
	.global int32
	.text

int32:
	jmp		REALPROG_ADDRESS    // just jump and realprog will handle everything
