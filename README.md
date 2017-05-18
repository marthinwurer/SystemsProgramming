# Systems Programming
Team 2: Benjamin Maitland, Brennan Ringey, Daniel Meiller, Nathan Castle

## Building

The OS can be built using the root Makefile. The default target is usb.image

To build and image the OS on a flash drive located at /dev/sdc:
```sh
make
sudo dd if=build/usb.image of=/dev/sdc && sync
```

For the DSL machines use:
```sh
make usb
```

Clean targets
 * clean: Removes object and dependency files but maintains directory structure
 * realclean: Same as clean, but removes directories and binaries

To clean everything:
```sh
make clean
# or
make realclean
```

To clean a certain project:
```sh
make kern-clean       # cleans only the kern folder in the build dir
# or 
make kern-realclean   # realcleans only the kern folder
```

## Configuration

All Makefile variables are defined in includes.default.mk. If you would like to
override any of these variables create an includes.mk file in the root directory
with the desired variable setting. The root Makefile includes the defaults first,
and the includes.mk second.

The following table contains all default variables:

| Name         | Default       | Description                                             |
|--------------|---------------|---------------------------------------------------------|
| BUILD_DIR    | ./build       | Location of the compiled object files and binaries      |
| VERBOSE      | 0             | Makefile verbosity: 0 = less verbose, 1 = most verbose  |
| CC           | gcc           | Command name of the C Compiler to use                   |
| CPP          | cpp           | Command name of the C Preproccessor to use              |
| AS           | as            | Command name of the Assembler to use                    |
| LD           | ld            | Command name of the linker to use                       |
| WERROR       |               | If set to -Werror, warnings are treated as errors       |
| WARNFLAGS    | -Wall -Wextra | Warning flags                                           |
| CFLAGS       | ...           | Flags used when compiling with CC                       |
| CPPFLAGS     | ...           | Flags used with CPP                                     |
| ASFLAGS      | ...           | Flags used wtih AS                                      |
| LDFLAGS      | ...           | Flags used with LD                                      |
| USER_OPTIONS | ...           | Compilation options                                     |

## User Options

The following user options exist and can be set for the USER_OPTIONS variable:
 * ISR_DEBUGGING_CODE  - includes context restore debugging code
 * CLEAR_BSS_SEGMENT   - include code to clear all BSS space in early init
 * SP_OS_CONFIG        - required, enables OS-specific startup variations
 * REPORT_MYSTERY_INTS - print a message on interrupt 0x27
 * DUMP_QUEUES         - dump information about scheduler queues periodically
 * NETWORK_ENABLED     - Enable the intel NIC driver
 * NETWORK_DUMP        - hexdump all received packets

## Project setup

| Directory | Description                           |
|-----------|---------------------------------------|
| baseline  | Baseline OS C and Assembly source     |
| doc       | Documentation                         |
| include   | Global header files                   |
| kern      | OS kernel C and Assembly source       |
| libc      | C Standard Library source             |
| man       | Man pages                             |
| mk        | Makefiles included from root Makefile |
| realprog  | realprog project source               |
| tools     | Utility applications used in building |
