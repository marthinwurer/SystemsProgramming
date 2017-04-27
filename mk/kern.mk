# mk/kern.mk
# Makefile for the kern and baseline folders

KERN_OBJ := early/_early.o \
            early/realmode.o \
            early/main.o \
            early/gdt_init.o \
            util/marquee.o \
            vesa/edid.o \
            vesa/vbe.o \
            video/color/color.o \
            video/fb/fb.o \
            video/globals.o \
            video/video.o

KERN_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(KERN_OBJ))

# Binary/source file for system bootstrap code

BOOT_OBJ := bootstrap.b
BOOT_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(BOOT_OBJ))

# Baseline object files

BASELINE_OBJ := startup.o \
                isr_stubs.o \
                klibs.o \
                ulibs.o \
                c_io.o \
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

BASELINE_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(BASELINE_OBJ))

# Collections of files

#KERN_OBJECTS = $(S_OBJ) $(C_OBJ) $(U_C_OBJ) $(U_S_OBJ)
PROG_OBJ := $(KERN_OBJ) $(BASELINE_OBJ)

$(BUILD_DIR)/prog.out: $(PROG_OBJ)
	$(LD) $(LDFLAGS) -o $@ $+

$(BUILD_DIR)/prog.o: $(PROG_OBJ) $(LIBK)
	$(LD) $(LDFLAGS) -o $@ -e _early -Ttext 0x10000 $+

$(BUILD_DIR)/prog.b: $(BUILD_DIR)/prog.o
	$(LD) $(LDFLAGS) -o $@ -s -e _early --oformat binary -Ttext 0x10000 $<

#$(BUILD_DIR)/earlyprog.o: $(EARLY_OBJ)
#	$(LD) $(LDFLAGS) -o $@ -Ttext 0x3000 -e _early $+

#$(BUILD_DIR)/earlyprog.b: $(BUILD_DIR)/earlyprog.o
#	$(LD) $(LDFLAGS) -o $@ -s --oformat binary -e _early -Ttext 0x3000 $<

# the -MD flags create dependency files when compiling, so we include them
# here (the - ignores errors if the file does not exist)
-include $(PROG_OBJ:.o=.d)