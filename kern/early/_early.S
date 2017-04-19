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


	/*
	** Early initialization complete!
	** Continue on to the kernel
	*/

	movl	$start_gdt, %esi
	movl	$GDT_ADDRESS, %edi
	movl	$(end_gdt - start_gdt), %ecx
	cld
	rep movsb

	lidt	idt_48
	lgdt	gdt_48

	ljmp	$GDT_CODE, $TARGET_ADDRESS


/*
** The GDTR and IDTR contents.
*/
gdt_48:
	.word	0x2000		/* 1024 GDT entries x 8 bytes/entry = 8192 */
	.quad	GDT_ADDRESS

idt_48:
	.word	0x0800		/* 256 interrupts */
	.quad	IDT_ADDRESS

start_gdt:
	.word	0,0,0,0		/* first GDT entry is always null */

linear_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF	/* limit[15:0] */
	.word	0x0000	/* base[15:0] */
	.byte	0x00	/* base[23:16] */
	.byte	0x92	/* access byte */
	.byte	0xCF	/* granularity */
	.byte	0x00	/* base[31:24] */

code_seg:	/* limit FFFFF, base 0, R/E code seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x9A	/* 1 00 1 1010: present, prio 0, C/D, R/E code */
	.byte	0xCF	/* 1 1 00 1111: 4K, 32-bit, 0, 0, limit[19:16] */
	.byte	0x00

data_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92	/* 1 00 1 0010: present, prio 0, C/D, R/W data */
	.byte	0xCF
	.byte	0x00

stack_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0xCF
	.byte	0x00
end_gdt: