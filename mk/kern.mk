# mk/kern.mk
# Makefile for the kern and baseline folders

U_C_OBJ := vga13/vga13.o vesa/vbe.o
U_C_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(U_C_OBJ))

U_S_OBJ := realmode.o
U_S_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(U_S_OBJ))

KERN_IO_OBJ := io/router.o io/device.o io/filesystem.o io/message.o io/middleware.o io/mount.o
KERN_IO_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(KERN_IO_OBJ))

# Binary/source file for system bootstrap code

BOOT_OBJ := bootstrap.b
BOOT_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(BOOT_OBJ))

# Assembly language object/source files

S_OBJ := startup.o \
         isr_stubs.o \
         klibs.o \
         ulibs.o 
S_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(S_OBJ))

# C object/source files

C_OBJ := c_io.o \
         support.o \
         clock.o \
         klibc.o \
         process.o \
         queue.o \
         scheduler.o \
         sio.o \
         stack.o \
         syscall.o \
         system.o \
         ulibc.o \
         user.o
C_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(C_OBJ))

# Collections of files

KERN_OBJECTS = $(S_OBJ) $(C_OBJ) $(U_C_OBJ) $(U_S_OBJ) $(KERN_IO_OBJ)

$(BUILD_DIR)/prog.out: $(KERN_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $+

$(BUILD_DIR)/prog.o: $(KERN_OBJECTS) $(LIBK)
	$(LD) $(LDFLAGS) -o $@ -Ttext 0x10000 $+

$(BUILD_DIR)/prog.b: $(BUILD_DIR)/prog.o
	$(LD) $(LDFLAGS) -o $@ -s --oformat binary -Ttext 0x10000 $<

# the -MD flags create dependency files when compiling, so we include them
# here (the - ignores errors if the file does not exist)
-include $(KERN_OBJECTS:.o=.d)
