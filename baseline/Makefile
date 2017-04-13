#
# SCCS ID: %W%	%G%
#
# Makefile to control the compiling, assembling and linking of
# standalone programs in the DSL.  Used for both individual
# interrupt handling assignments and the SP baseline OS (with
# appropriate tweaking).
#

#
# User supplied files
#
SYS_C_SRC = clock.c klibc.c process.c queue.c scheduler.c sio.c \
	stack.c syscall.c system.c

SYS_C_OBJ = clock.o klibc.o process.o queue.o scheduler.o sio.o \
	stack.o syscall.o system.o

SYS_S_SRC = klibs.S

SYS_S_OBJ = klibs.o

SYS_SRCS = $(SYS_C_SRC) $(SYS_S_SRC)
SYS_OBJS = $(SYS_C_OBJ) $(SYS_S_OBJ)

USR_C_SRC = ulibc.c user.c

USR_C_OBJ = ulibc.o user.o

USR_S_SRC = ulibs.S

USR_S_OBJ = ulibs.o

USR_SRCS = $(USR_C_SRC) $(USR_S_SRC)
USR_OBJS = $(USR_C_OBJ) $(USR_S_OBJ)

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP_OS_CONFIG		enable SP OS-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#	DUMP_QUEUES		clock ISR dumps queues every 10 seconds
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP_OS_CONFIG -DDUMP_QUEUES

#
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
#
# Compilation/assembly control
#

#
# We only want to include from the current directory and ~wrc/include
#
INCLUDES = -I. -I/home/fac/wrc/include

#
# Compilation/assembly/linking commands and options
#
CPP = cpp
# CPPFLAGS = $(USER_OPTIONS) -nostdinc -I- $(INCLUDES)
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES)

CC = gcc
CFLAGS = -m32 -std=c99 -fno-stack-protector -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS = --32

LD = ld
LDFLAGS = -melf_i386

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
# use of #include/#define/#ifdef statements. However, the line numbers of
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
#

.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

.s.b:
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(LD) $(LDFLAGS) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# Binary/source file for system bootstrap code

BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S

# Assembly language object/source files

FMK_S_OBJ = startup.o isr_stubs.o $(U_S_OBJ)
FMK_S_SRC =	startup.S isr_stubs.S $(U_S_SRC)

# C object/source files

FMK_C_OBJ =	c_io.o support.o $(U_C_OBJ)
FMK_C_SRC =	c_io.c support.c $(U_C_SRC)

FMK_OBJS = $(FMK_S_OBJ) $(FMK_C_OBJ)
FMK_SRCS = $(FMK_S_SRC) $(FMK_C_SRC)

# System file collections

OBJECTS = $(FMK_OBJS) $(SYS_OBJS) $(USR_OBJS)
SOURCES = $(FMK_SRCS) $(SYS_SRCS) $(USR_SRCS)

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

usb.image: bootstrap.b prog.b prog.nl BuildImage prog.dis  # Offsets
	./BuildImage -d usb -o usb.image -b bootstrap.b prog.b 0x10000

floppy.image: bootstrap.b prog.b prog.nl BuildImage #prog.dis 
	./BuildImage -d floppy -o floppy.image -b bootstrap.b prog.b 0x10000

prog.out: $(OBJECTS)
	$(LD) $(LDFLAGS) -o prog.out $(OBJECTS)

prog.o:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

prog.b:	prog.o
	$(LD) $(LDFLAGS) -o prog.b -s --oformat binary -Ttext 0x10000 prog.o

#
# Targets for copying bootable image onto boot devices
#

floppy:	floppy.image
	dd if=floppy.image of=/dev/fd0

usb:	usb.image
	/usr/local/dcs/bin/dcopy usb.image

#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c

# NOT BUILDING CORRECTLY ON x86_64 SYSTEMS
# Offsets:	Offsets.c
# 	$(CC) -std=c99 -mx32 $(INCLUDES) -o Offsets Offsets.c

#
# Clean out this directory
#

clean:
	rm -f *.nl *.na *.s *.lst *.b *.o *.image *.dis BuildImage Offsets

#
# Create a printable namelist from the prog.o file
#

prog.nl: prog.o
	nm -Bng prog.o | pr -w80 -3 > prog.nl
	nm -Bn  prog.o | pr -w80 -3 > prog.na

#
# Generate a disassembly
#

prog.dis: prog.o
	objdump -d prog.o > prog.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

startup.o: bootstrap.h
isr_stubs.o: bootstrap.h
c_io.o: c_io.h startup.h support.h /home/fac/wrc/include/x86arch.h
support.o: startup.h support.h c_io.h /home/fac/wrc/include/x86arch.h
support.o: bootstrap.h
clock.o: /home/fac/wrc/include/x86arch.h startup.h common.h types.h c_io.h
clock.o: support.h system.h bootstrap.h process.h stack.h queue.h ulib.h
clock.o: klib.h clock.h scheduler.h sio.h syscall.h
klibc.o: common.h types.h c_io.h support.h system.h
klibc.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
klibc.o: queue.h ulib.h klib.h
process.o: common.h types.h c_io.h support.h system.h
process.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
process.o: queue.h ulib.h klib.h
queue.o: common.h types.h c_io.h support.h system.h
queue.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
queue.o: queue.h ulib.h klib.h
scheduler.o: common.h types.h c_io.h support.h system.h
scheduler.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
scheduler.o: queue.h ulib.h klib.h scheduler.h clock.h
sio.o: common.h types.h c_io.h support.h system.h
sio.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h queue.h
sio.o: ulib.h klib.h sio.h scheduler.h startup.h /home/fac/wrc/include/uart.h
stack.o: common.h types.h c_io.h support.h system.h
stack.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
stack.o: queue.h ulib.h klib.h
syscall.o: common.h types.h c_io.h support.h system.h
syscall.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
syscall.o: queue.h ulib.h klib.h /home/fac/wrc/include/uart.h startup.h
syscall.o: syscall.h scheduler.h clock.h sio.h
system.o: common.h types.h c_io.h support.h system.h
system.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
system.o: queue.h ulib.h klib.h clock.h syscall.h sio.h scheduler.h user.h
ulibc.o: common.h types.h c_io.h support.h system.h
ulibc.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
ulibc.o: queue.h ulib.h klib.h
user.o: common.h types.h c_io.h support.h system.h
user.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h queue.h
user.o: ulib.h klib.h user.h
ulibs.o: syscall.h common.h types.h c_io.h support.h system.h
ulibs.o: /home/fac/wrc/include/x86arch.h bootstrap.h process.h stack.h
ulibs.o: queue.h ulib.h klib.h
