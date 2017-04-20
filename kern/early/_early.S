/*
** File: _early.S
**
** Author: Brennan Ringey (bjr1251)
**
** Early kernel initialization code. The bootstrap transfers control here
** after switching to protected mode. Once initialization is done, control
** is then transferred to the kernel.
*/


#include <baseline/bootstrap.h>

	.arch i386
	.code32

	.globl main, init_int32

	.globl  begtext, _early
	.text

begtext:
_early:
	cli
	xorl	%eax, %eax
	movw	$EARLY_GDT_DATA32, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	movl	$TARGET_STACK, %ebp
	movl	%ebp, %esp

	// initialize the int32 function
	call	init_int32

	// call main
	call	main

	// initialize the kernel GDT
	call	gdt_init

	// Load the IDT and GDT registers
	lidt	idt_48
	lgdt	gdt_48

	/*
	** Early initialization complete!
	** Continue on to the kernel
	*/
	ljmp	$GDT_CODE, $TARGET_ADDRESS

	/*movl	$start_gdt, %esi
	movl	$GDT_ADDRESS, %edi
	movl	$(end_gdt - start_gdt), %ecx
	cld
	rep movsb*/

	


/*
** The GDTR and IDTR contents.
*/
gdt_48:
	.word	0x2000		/* 1024 GDT entries x 8 bytes/entry = 8192 */
	.quad	GDT_ADDRESS

idt_48:
	.word	0x0800		/* 256 interrupts */
	.quad	IDT_ADDRESS


