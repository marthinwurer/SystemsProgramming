/*
** File: realprog/realprog.S
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains the actual implementation of the int32 support function. This
** program is loaded at 0x3000. The int32 function jumps here and this program
** will do the return.
**
*/

#include <baseline/bootstrap.h>
#define REALPROG_STACK_SIZE 44

	.globl begtext
	.text
	.code32


begtext:
	pusha
	movl	%esp, __stack32_ptr

	leal	0x24(%esp), %esi                 // copy interrupt number to immediate byte label
	lodsl
	movb	%al, __ib

	movl    (%esi), %esi                     // copy regs argument to realprog stack
	movl    $__stack, %edi
	movl    $REALPROG_STACK_SIZE, %ecx
	movl    %edi, %esp
	rep movsb
	ljmpw	$EARLY_GDT_CODE16, $__pmode16
__pmode16:
		.code16
	movw	$EARLY_GDT_DATA16, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	movl	%cr0, %eax
	andb	$(~1), %al
	movl	%eax, %cr0
	jmp		$0, $__rmode16
__rmode16:
	xorw	%ax, %ax
	movw	%ax, %ds
	movw	%ax, %ss
	popal
	pop		%gs
	pop		%fs
	pop		%es
	pop		%ds
	sti
		.byte 0xCD
__ib:	.byte 0x0
	cli
	xorw	%sp, %sp
	movw	%sp, %ss
	movw	$__stackend, %sp
	pushfl
	pushw	%ds
	pushw	%es
	pushw	%fs
	pushw	%gs
	pushal
	movl	%cr0, %eax
	orl		$1, %eax
	movl	%eax, %cr0
	ljmpl	$EARLY_GDT_CODE32, $__pmode32
__pmode32: .code32
	xorl	%eax, %eax
	movw	$EARLY_GDT_DATA32, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	movl	__stack32_ptr, %esp
	movl	$__stack, %esi
	leal	0x28(%esp), %edi
	movl	(%edi), %edi
	movl	$REALPROG_STACK_SIZE, %ecx
	cld
	rep movsb
	popa
	ret

__stack32_ptr:
	.long 0x0


/*
** The end of this program contains the stack which contain the
** registers after executing the int instructions, and the location of
** the immediate byte in the int instruction.
*/

	.org 512 - REALPROG_STACK_SIZE - 2

__stack:
	.fill 8, 4, 0
	.fill 4, 2, 0
	.fill 1, 4, 0
__stackend:

__ib_loc:
	.word __ib

