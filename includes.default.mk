
# Makefile containing the default configuration variables
# This file is used by the root Makefile if includes.mk does
# not exist.

# location of the build directory
BUILD_DIR := build

# makefile verbosity
# 0: Only show command variable and output file
# 1: Show entire command
VERBOSE := 0

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

WARNFLAGS := -Wall -Wextra

CFLAGS = -std=c99 -fno-stack-protector -fno-builtin $(WARNFLAGS) $(WERROR)

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
#
# USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP_OS_CONFIG -DDUMP_QUEUES
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP_OS_CONFIG -DNETWORK_ENABLED -DNETWORK_DUMP