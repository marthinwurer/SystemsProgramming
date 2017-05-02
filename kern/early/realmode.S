
#include <baseline/bootstrap.h>

	.arch i386

	#.global int32, init_int32
	.global int32
	.text

int32:
	jmp		REALPROG_ADDRESS    // just jump and realprog will handle everything
