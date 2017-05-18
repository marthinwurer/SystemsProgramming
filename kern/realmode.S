/*
** File: kern/realmode.S
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** DEPRECATED!
** This is the old int32 utility function. File does not get compiled.
** Do not use.
**
*/

	.arch i386
	.globl __init_int32, __int32


#include <baseline/bootstrap.h>

#define REALPROG_ADDRESS       0x3000
#define REALPROG_STACK_ADDRESS 0x2FE6
#define REALPROG_STACK_SIZE 26
#define REBASE(x) (((x) - __realprog) + REALPROG_ADDRESS)

#define CODE16 0x28
#define DATA16 0x30
#define CODE32 0x10
#define DATA32 0x18

	.text
	.code32
/* 
** __int32_init function
** Must be called before using __int32. Copies the real program code at the
** address specified in INT32_BASE
*/
__init_int32:
	movl $__realprog, %esi
	movl $REALPROG_ADDRESS, %edi
	movl $(__realprog_end - __realprog), %ecx

	cld
	rep movsb
	ret

__int32:
	cli
	pusha
	movl    %esp, REBASE(__stack32_ptr)           // save 32bit stack pointer
	movl    %cr0, %eax
	movl    %eax, REBASE(__cr0_ptr)
	sidt    REBASE(__idt32_ptr)                   // save 32bit idt register
	//sgdt    REBASE(__gdt32_ptr)                   // save 32bit gdt register

	// disable NMIs
	movl    $0x70, %edx
	inb     (%dx)
	orl     $0x80, %edx

	movl    %edx, %eax
	movl    $0x70, %edx
	outb    (%dx)

	// copy interrupt number to immediate byte label
	leal    0x24(%esp), %esi
	lodsl
	movb    %al, REBASE(__ib)

	// copy regs argument to realprog stack
	movl    (%esi), %esi
	movl    $REALPROG_STACK_ADDRESS, %edi
	movl    $REALPROG_STACK_SIZE, %ecx
	movl    %edi, %esp
	rep movsb
	jmp     REALPROG_ADDRESS
__realprog:
	//lgdt    REBASE(__real_gdt)                    // load 16bit gdt
	ljmpw   $GDT_CODE16, $REBASE(__pmode16)
__pmode16: .code16
	movw    $GDT_DATA16, %ax
	movw    %ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	lidt    REBASE(__real_idt)
	movl	%cr0, %eax					// turn off PE bit (to leave protected mode)
	andb	$(~1), %al					// mask off PE bit
	movl	%eax, %cr0					// set cr0 to result
	jmp     $0x0, $REBASE(__rmode16)
__rmode16:
	xorw    %ax, %ax
	movw    %ax, %ds
	movw    %ax, %ss
	movw	$0x870, %bx					// master 8 slave 112
	call	resetpic					// set pic's to real mode settings
	popa
	pop     %gs
	pop     %fs
	pop     %es
	pop     %ds
	sti
	  .byte 0xCD
__ib: .byte 0x0
    cli
	xorw    %sp, %sp
	movw    %sp, %ss
	movw    $REALPROG_ADDRESS, %sp
	pushf
	pushw   %ds
	pushw   %es
	pushw   %fs
	pushw   %gs
	pusha
	movw	$0x2028, %bx					// master 8 slave 112
	call	resetpic					// set pic's to real mode settings
	movl    REBASE(__cr0_ptr), %eax
	//movl    %eax, %cr0
	//movl    %cr0, %eax
	orl     $1, %eax
	movl    %eax, %cr0
	ljmpl    $GDT_CODE, $REBASE(__pmode32)
__pmode32: .code32
	movw    $GDT_DATA, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	movw    $GDT_STACK, %ax
	movw    %ax, %ss
	lidt    REBASE(__idt32_ptr)
	movl    REBASE(__stack32_ptr), %esp
	movl    $REALPROG_STACK_ADDRESS, %esi
	leal    0x28(%esp), %edi
	movl    (%edi), %edi
	movl    $REALPROG_STACK_SIZE, %ecx
	cld
	rep movsb
	
	// re-enable NMIs
	movl    $0x70, %edx
	inb     (%dx)
	andl    $0x7F, %edx

	movl    %edx, %eax
	movl    $0x70, %edx
	outb    (%dx)

	//movl    REBASE(__cr0_ptr), %eax
	//movl    %eax, %cr0

	// restore gp regs
	popa
	sti
	ret

resetpic: .code16
	pushw	%ax
	movb	$0x11, %al
	outb	%al, $0x20
	outb	%al, $0xA0
	movb	%bh, %al
	outb	%al, $0x21
	movb	%bl, %al
	outb	%al, $0xA1
	movb	$0x04, %al
	outb	%al, $0x21
	shrb	$1, %al
	outb	%al, $0xA1
	shrb	$1, %al
	outb	%al, $0x21
	outb	%al, $0xA1
	popw	%ax
	ret
.code32

__stack32_ptr:
	.long 0x0

__idt32_ptr:
	.word 0x0
	.long 0x0

__cr0_ptr:
	.long 0x0

// the best idt, the real idt

__real_idt:
	.word	0x3FF
	.long	0x0

// the best gdt, the real gdt
// Note that this GDT will have the same layout as the bootstrap GDT, this way
// we won't have to change the segment registers
/*
__real_gdt_start:
	.word 0,0,0,0                // null entry

__real_gdt_code32:
	.word 0xFFFF
	.word 0x0000
	.byte 0x00
	.byte 0x9A
	.byte 0xCF
	.byte 0x00

__real_gdt_data32:
	.word 0xFFFF
	.word 0x0000
	.byte 0x0
	.byte 0x92
	.byte 0xCF
	.byte 0x00

__real_gdt_code16:                 // 16-bit code entry
	.word   0xFFFF
	.word   0x0000
	.byte   0x00 
	.byte   0x9A
	.byte   0x0F
	.byte   0x00

__real_gdt_data16:
	.word   0xFFFF
	.word   0x0000
	.byte   0x00
	.byte   0x92
	.byte   0x0F
	.byte   0x00

__real_gdt:
	.word	(__real_gdt - __real_gdt_start) - 1
	.long	REBASE(__real_gdt_start)
*/
__realprog_end:
