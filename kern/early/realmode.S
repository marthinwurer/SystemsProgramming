
#include <baseline/bootstrap.h>

#define REALPROG_STACK_SIZE 26
#define REBASE(x) (((x) - __realprog) + REALPROG_ADDRESS)

	.arch i386

	.global int32, init_int32

init_int32:
	movl	$__realprog, %esi
	movl	$REALPROG_ADDRESS, %edi
	movl	$(__realprog_end - __realprog), %ecx
	cld
	rep movsb
	ret

int32:
	pusha
	movl    %esp, REBASE(__stack32_ptr)           // save 32bit stack pointer
	sidt    REBASE(__idt32_ptr)

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
	ljmpw   $EARLY_GDT_CODE16, $REBASE(__pmode16)
__pmode16: .code16
	movw    $EARLY_GDT_DATA16, %ax
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
	movl    %cr0, %eax
	orl     $1, %eax
	movl    %eax, %cr0
	ljmpl   $EARLY_GDT_CODE32, $REBASE(__pmode32)
__pmode32: .code32
	movw    $EARLY_GDT_DATA32, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	movw    %ax, %ss
	lidt    REBASE(__idt32_ptr)
	movl    REBASE(__stack32_ptr), %esp
	movl    $REALPROG_STACK_ADDRESS, %esi
	leal    0x28(%esp), %edi
	movl    (%edi), %edi
	movl    $REALPROG_STACK_SIZE, %ecx
	cld
	rep movsb
	// restore gp regs
	popa
	sti
	ret

__stack32_ptr:
	.long 0x0

__idt32_ptr:
	.word 0x0
	.long 0x0

// the best idt, the real idt

__real_idt:
	.word	0x3FF
	.long	0x0

__realprog_end:

/*	pusha
	movw	%sp, %bp
	//addw	$0x2A, %bx
	movw	0x2A(%bp), %ax
	movb	%al, __ib
	subw	$16, %sp              // point %sp to the 
	popa
	pop		%gs
	pop		%fs
	pop		%es
	pop		%ds
	sti
      .byte 0xCD
__ib: .byte 0x0
	cli
	pushf
	pushw	%ds
	pushw	%es
	pushw	%fs
	pushw	%gs
	pusha
	addw	$16, %sp
	pusha
	ret
*/