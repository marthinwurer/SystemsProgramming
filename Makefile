
# if BUILD_DIR is not specified, default to ./build
BUILD_DIR ?= build

U_C_OBJ := main.o vga13/vga13.o
U_C_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(U_C_OBJ))

U_S_OBJ :=
U_S_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(U_S_OBJ))


#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP_OS_CONFIG		enable SP OS-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT

#
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
#
# Compilation/assembly control
#

.SECONDEXPANSION:

# marker file to create when making directory
MARKER_FILE := .marker

# Marker prereq
MARKER = $$(@D)/$(MARKER_FILE)


%/$(MARKER_FILE):
	mkdir -p $(dir $@)
	touch $@

.PRECIOUS: %/$(MARKER_FILE)

#
# We only want to include from the include directory
#
INCLUDES = -Iinclude

# directory of the BuildImage program source
BUILDIMAGE_DIR := BuildImage
# path to the BuildImage executable
BUILDIMAGE := $(BUILDIMAGE_DIR)/BuildImage

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

# transformation rules

$(BUILD_DIR)/%.s: %.c $(MARKER)
	$(CC) $(CFLAGS) -o $@ -S $<

$(BUILD_DIR)/%.s: %.S $(MARKER)
	$(CPP) $(CPPFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.S $(MARKER)
	$(CPP) $(CPPFLAGS) -o $(BUILD_DIR)/$*.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.s -a=$(BUILD_DIR)/$*.lst
	$(RM) -f $*.s

$(BUILD_DIR)/%.b: %.s $(MARKER)
	$(AS) $(ASFLAGS) -o $(BUILD_DIR)/$*.o $< -a=$(BUILD_DIR)/$*.lst
	$(LD) $(LDFLAGS) -Ttext 0x0 -s --oformat binary -e begtext -o $@ $(BUILD_DIR)/$*.o

$(BUILD_DIR)/%.o: %.c $(MARKER)
	$(CC) $(CFLAGS) -o $@ -c $<

# Binary/source file for system bootstrap code

BOOT_OBJ := bootstrap.b
BOOT_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(BOOT_OBJ))

# Assembly language object/source files

S_OBJ := startup.o isr_stubs.o 
S_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(S_OBJ)) $(U_S_OBJ)

# C object/source files

C_OBJ :=	c_io.o support.o
C_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(C_OBJ)) $(U_C_OBJ)

# Collections of files

OBJECTS = $(S_OBJ) $(C_OBJ)

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

IMAGE_FILES := $(BUILD_DIR)/baseline/bootstrap.b \
               $(BUILD_DIR)/prog.b

$(BUILD_DIR)/usb.image: $(IMAGE_FILES) $(BUILDIMAGE) #prog.dis 
	$(BUILDIMAGE) -d usb -o $@ -b $(IMAGE_FILES) 0x10000

$(BUILD_DIR)/floppy.image: $(IMAGE_FILES) $(BUILDIMAGE) #prog.dis 
	$(BUILDIMAGE) -d floppy -o $@ -b $(IMAGE_FILES) 0x10000

$(BUILD_DIR)/prog.out: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/prog.o: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

$(BUILD_DIR)/prog.b: $(BUILD_DIR)/prog.o
	$(LD) $(LDFLAGS) -o $@ -s --oformat binary -Ttext 0x10000 $<


#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

$(BUILDIMAGE):
	cd $(BUILDIMAGE_DIR) && $(MAKE)

Offsets:	Offsets.c
	$(CC) $(INCLUDES) -o Offsets Offsets.c
