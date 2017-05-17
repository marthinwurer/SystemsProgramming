# mk/kern.mk
# Makefile for the kern and baseline folders

# U_C_OBJ := vga13/vga13.o vesa/vbe.o
# U_C_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(U_C_OBJ))

# U_S_OBJ := realmode.o
# U_S_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(U_S_OBJ))

KERN_DRV_OBJ := ramdisk/ramdisk.o rawfs/raw.o fat32/fat.o fat32/pubfat.o fat32/time.o
KERN_DRV_OBJ := $(addprefix $(BUILD_DIR)/kern/drivers/, $(KERN_DRV_OBJ))
KERN_API_OBJ := simple_mount.o file.o
KERN_API_OBJ := $(addprefix $(BUILD_DIR)/kern/ioapi/, $(KERN_API_OBJ))
KERN_IO_OBJ := io/router.o io/device.o io/filesystem.o io/message.o io/middleware.o io/mount.o io/memshim.o 
KERN_IO_OBJ := $(addprefix $(BUILD_DIR)/kern/,$(KERN_IO_OBJ))
# EARLY_OBJ := _early.o realmode.o main.o gdt_init.o
# EARLY_OBJ := $(addprefix $(BUILD_DIR)/kern/early/,$(EARLY_OBJ))

KERN_OBJ := early/_early.o \
            early/realmode.o \
            early/main.o \
            early/gdt_init.o \
            pci/pci.o \
            net/intel.o \
            net/net_test.o \
            early/memory_map_setup.o \
            early/video.o \
            graphics/shapes/rect.o \
            graphics/text/text.o \
            memory/memory_map.o \
            util/marquee.o \
            vconsole/buffer.o \
            vconsole/console.o \
            vconsole/control.o \
            vconsole/render/draw.o \
            vconsole/render/scroll.o \
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
                user.o \
				prettyprinter.o

BASELINE_OBJ := $(addprefix $(BUILD_DIR)/baseline/,$(BASELINE_OBJ))

# Collections of files

#KERN_OBJECTS = $(S_OBJ) $(C_OBJ) $(U_C_OBJ) $(U_S_OBJ) $(KERN_IO_OBJ) $(KERN_DRV_OBJ) $(KERN_API_OBJ)
PROG_OBJ := $(KERN_OBJ) $(KERN_IO_OBJ) $(KERN_DRV_OBJ) $(BASELINE_OBJ) $(KERN_API_OBJ)

$(BUILD_DIR)/prog.out: $(PROG_OBJ)
	$(LD_V) $(LDFLAGS) -o $@ $+

$(BUILD_DIR)/prog.o: $(PROG_OBJ) $(LIBK)
	$(LD_V) $(LDFLAGS) -o $@ -e _early -Ttext 0x10000 $+

$(BUILD_DIR)/prog.b: $(BUILD_DIR)/prog.o
	$(LD_V) $(LDFLAGS) -o $@ -s -e _early --oformat binary -Ttext 0x10000 $<

#$(BUILD_DIR)/earlyprog.o: $(EARLY_OBJ)
#	$(LD) $(LDFLAGS) -o $@ -Ttext 0x3000 -e _early $+

#$(BUILD_DIR)/earlyprog.b: $(BUILD_DIR)/earlyprog.o
#	$(LD) $(LDFLAGS) -o $@ -s --oformat binary -e _early -Ttext 0x3000 $<

# the -MD flags create dependency files when compiling, so we include them
# here (the - ignores errors if the file does not exist)
-include $(PROG_OBJ:.o=.d)
