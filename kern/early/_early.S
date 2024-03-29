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

	.globl main, init_int32, c_io_init, c_puts, c_printf

	.globl _start

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

#ifdef CLEAR_BSS_SEGMENT

/*
** Zero the BSS segment
**
** These symbols are defined automatically by the linker.
*/
	.globl	__bss_start,_end

	movl	$__bss_start,%edi
clearbss:
	movl	$0,(%edi)
	addl	$4,%edi
	cmpl	$_end,%edi
	jb	clearbss

#endif

	// initialize c_io
	call	c_io_init
	
	pushl	$enterMsg
	call	c_puts

	// call main
	call	main
	cmpl	$0, %eax
	jnz		__early_fail
	
	// initialize the kernel GDT
	call	gdt_init

	// Load the IDT and GDT registers
	lidt	idt_48
	lgdt	gdt_48

	pushl	$leaveMsg
	call	c_puts

	/*
	** Early initialization complete!
	** Continue on to the kernel
	*/
	ljmp	$GDT_CODE, $_start

	/*movl	$start_gdt, %esi
	movl	$GDT_ADDRESS, %edi
	movl	$(end_gdt - start_gdt), %ecx
	cld
	rep movsb*/

__early_fail:
	pushl	%eax
	pushl	$failMsg
	call	c_printf
__early_fail_loop:
	hlt
	jmp		__early_fail_loop

enterMsg:
	.asciz "=== Entered early init ===\n"

leaveMsg:
	.asciz "=== Leaving early init ===\n"

failMsg:
	.asciz "error: main() returned %d\nHalting...\n"


/*
** The GDTR and IDTR contents.
*/
gdt_48:
	.word	0x2000		/* 1024 GDT entries x 8 bytes/entry = 8192 */
	.quad	GDT_ADDRESS

idt_48:
	.word	0x0800		/* 256 interrupts */
	.quad	IDT_ADDRESS


