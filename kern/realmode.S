/*
** File: kern/realmode.S
**
** Author: Brennan Ringey
**
** Based off of Napalm's Protected Mode BIOS Call Functionality v2.0
** http://www.rohitab.com/discuss/topic/35103-switch-between-real-mode-and-protected-mode/?p=10071826
** License: https://creativecommons.org/licenses/by-sa/2.0/uk/
**
**/
	.arch i386

	.global __init_int32, __int32


// sizeof the regs16_t type, 13 16-bit registers = 26 bytes
#define REGS16_SIZE 26

// location to real mode program (overwrites the bootloader)
#define INT32_BASE 0x7C00
#define REBASE(x) (((x) - reloc) + INT32_BASE)
#define GDTENTRY(x) (x << 3)
#define CODE32 GDTENTRY(1)
#define DATA32 GDTENTRY(2)
#define CODE16 GDTENTRY(3)
#define DATA16 GDTENTRY(4)

// 13 = 13 registers in regs16_t x 2 = 26 bytes needed for stack
#define STACK16 (INT32_BASE - REGS16_SIZE)



	.text
	.code32
/* 
** __int32_init function
** Must be called before using __int32. Copies the real program code at the
** address specified in INT32_BASE
*/
__init_int32:
	//enter $0,$0
	movl $reloc, %esi
	movl $INT32_BASE, %edi
	movl $(__int32_end - reloc), %ecx

	cld
	rep movsb
	
	//leave
	ret

testmsg:
	.string "%d\n"



/*
**  __int32 support function
** Temporarily changes back to real mode to execute a BIOS service
** routine using the int instruction.
*/
__int32:
	// Clear interrupts
	cli
	pusha
	jmp INT32_BASE
// the following is relocated to the bootloader by __init_int32, 
// hence the reloc label
reloc:
	movl	%esp, REBASE(stack32_ptr)	// save 32bit stack pointer
	sidt	REBASE(idt32_ptr)			// save 32bit idt register
	sgdt	REBASE(gdt32_ptr)			// save 32bit gdt register
	lgdt	REBASE(gdt16_ptr)			// load 16bit gdt
	leal	0x24(%esp), %esi			// set position of vector on stack
	lodsl								// read vector into eax
	movb	%al, REBASE(ib)				// set interrupt immediate byte
	movl	(%esi), %esi				// read regs pointer in esi as source
	movl	$STACK16, %edi				// set destination to 16bit stack location
	movl	$REGS16_SIZE, %ecx			// set copy size to struct size
	movl	%edi, %esp					// save dest to as 16bit stack offset
	rep movsb							// do the copy (32bit stack to 16bit)
	ljmpw	$CODE16, $REBASE(p_mode16)	// switch to 16bit selector
p_mode16: .code16
	movw	$DATA16, %ax				// 16-bit data selector into ax
	movw	%ax, %ds					// set all segment regs to ax
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	movl	%cr0, %eax					// turn off PE bit (to leave protected mode)
	andb	$(~1), %al					// mask off PE bit
	movl	%eax, %cr0					// set cr0 to result
	jmp		$0x0, $REBASE(r_mode16)		// got to real mode
r_mode16:
	xorw	%ax, %ax					// clear %ax
	movw	%ax, %ds					// set %ds so we can access idt16
	movw	%ax, %ss					// set %ss so the stack is valid
	lidt	REBASE(idt16_ptr)			// load 16bit IDT
	movw	$0x870, %bx					// master 8 slave 112
	call	resetpic					// set pic's to real mode settings
	popa								// load gp regs from 16-bit stack
	pop		%gs							// load gs
	pop		%fs							// load fs
	pop		%es							// load es
	pop		%ds							// load ds
	sti									// enable interrupts
//ib:	int $0
	.byte	0xCD						// int instruction byte
ib: .byte	0x0						// int instruction operand
	cli									// disable interrupts
	xorw	%sp, %sp					// clear %sp
	movw	%sp, %ss					// set %ss so stack is valid
	movw	$INT32_BASE, %sp			// set position so we can copy back
	pushf								// save eflags to 16bit stack
	pushw	%ds							// save %ds to 16bit stack
	pushw	%es							// save %es to 16bit stack
	pushw	%fs							// save %fs to 16bit stack
	pushw	%gs							// save %gs to 16bit stack
	pusha								// save gp regs to 16bit stack
	movw	$0x2028, %bx				// master 32, slave 40
	call	resetpic					// restore the pic's to pmode settings
	movl	%cr0, %eax					// get cr0 to enable PE bit
	inc		%eax						// enable PE bit
	movl	%eax, %cr0					// set cr0 to result
	ljmpl	$CODE32, $REBASE(p_mode32)	// switch to 32bit selector
p_mode32: .code32
	movw	$DATA32, %ax				// get 32bit data selector
	movw	%ax, %ds					// reset ds selector
	movw	%ax, %es					// reset es selector
	movw	%ax, %fs					// reset fs selector
	movw	%ax, %gs					// reset gs selector
	movw	%ax, %ss					// reset ss selector
	lgdt	REBASE(gdt32_ptr)			// restore 32bit GDT
	lidt	REBASE(idt32_ptr)			// restore 32bit IDT
	movl	REBASE(stack32_ptr), %esp	// restore 32bit stack pointer
	movl	$STACK16, %esi				// set copy source to 16bit stack
	leal	0x28(%esp), %edi			// use position of regs pointer on stack
	movl	(%edi), %edi				// use regs pointer in edi as copy dest
	movl	$REGS16_SIZE, %ecx			// set copy size to size of struct
	cld									// clear director flag, copy forward
	rep movsb							// do the copy (16bit stack to 32bit stack)
	popa								// restore gp regs
	sti									// enable interrupts
	ret									// return to caller

resetpic:
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

stack32_ptr:
	.long 0x0

idt32_ptr:
	.word 0x0
	.long 0x0

gdt32_ptr:
	.word 0x0
	.long 0x0

idt16_ptr:
	.word 0x3FF
	.long 0x0

gdt16_base:
	.word 0, 0, 0, 0

	gdt16_code32:
		.word 0xFFFF
		.word 0x0000
		.byte 0x00
		.byte 0x9A
		.byte 0xCF
		.byte 0x00
	
	gdt16_data32:
		.word 0xFFFF
		.word 0x0000
		.byte 0x0
		.byte 0x92
		.byte 0xCF
		.byte 0x00
	
	gdt16_code16:
		.word 0xFFFF
		.word 0x0000
		.byte 0x00
		.byte 0x9A
		.byte 0x0F
		.byte 0x00
	
	gdt16_data16:
		.word 0xFFFF
		.word 0x0000
		.byte 0x00
		.byte 0x92
		.byte 0x0F
		.byte 0x00

gdt16_ptr:
	.word gdt16_ptr - gdt16_base - 1
	.long gdt16_base


__int32_end:


/*
; 	; paging is assumed to be disabled or not set up yet
; 	; otherwise we will have to disable it here

; 	pusha	; save register state

; 	; save the current state of the gdt register
; 	sgdt	__oldgdt

; 	; load the gdt register with the gdt we will use will in real mode
; 	lgdt	__real_gdt

; 	; far jump into 16-bit protected mode
; 	; in this case we will jump to where the bootloader was
; 	; __init_int32 will overwrite it with the real mode program
; 	ljmp	0x0, 0x7C00

; 	; we will return here from real mode
; 	; restore everything and return to protected mode

; 	; restore the old gdt
; 	lgdt	__oldgdt

; 	; restore register state
; 	popa

; 	; re-enable nmi


; 	; set interrupts
; 	sti
	
; 	; return, we done
; 	ret



; 	; load data segment selectors with 16-bit segment index

; 	; save current idt
; 	sidt	__oldidt

; 	; load real mode idt
; 	lidt	__real_idt


; ;; the best idt, the real idt

; __real_idt:
; 	.word	0x3FF
; 	.quad	0x0

; ;; the best gdt, the real gdt

; __real_gdt:
; 	.word	23		; 3 entries, 3 x 8 bytes/entry - 1 = 23
; 	.quad	__real_gdt_start

; __real_gdt_start:
; 	.word 0,0,0,0

; __real_gdt_data:
; 	.word	0xFFFF
; 	.word	0x0000
; 	.word	0x9200
; 	.word	0x000F

; __real_gdt_code:
; 	.word	0xFFFF
; 	.word	0x0000
; 	.word	0x9A00  ; only difference here is that EX = 1
; 	.word	0x000F
*/