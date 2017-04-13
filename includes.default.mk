# Makefile containing the default configuration variables
# This file is used by the root Makefile if includes.mk does
# not exist.

# location of the build directory
BUILD_DIR := build


# =============================================================================
# COMMANDS
# =============================================================================

CC := gcc

CPP := cpp

AS := as

LD := ld

# =============================================================================
# FLAGS
# =============================================================================

# if set warnings will be treated as errors
WERROR :=

WARNFLAGS := -Wall -Wextra -pedantic

CFLAGS = -m32 -std=c99 -fno-stack-protector -fno-builtin $(WARNFLAGS) $(WERROR)

CPPFLAGS = -nostdinc -I$(INC_DIR)

ASFLAGS = --32

LDFLAGS = -melf_i386

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP_OS_CONFIG		enable SP OS-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#	BOOT_ENABLE_VGA13	bootstrap will enter VGA Mode 0x13 before entering
#                       protected mode
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP_OS_CONFIG -DDUMP_QUEUES