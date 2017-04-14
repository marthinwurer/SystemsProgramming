
include mk/marker.mk

include mk/constants.mk

include includes.default.mk

# include includes.mk if it exists
ifneq ($(wildcard includes.mk),)
include includes.mk
endif

# Default target (BUILD_DIR/usb.image)
all: $(BUILD_DIR)/usb.image


# Project makefiles
# Include 'em here

PROJECTS := kern baseline libc

# C Standard library for kernel and userspace
include mk/libc.mk

# The kernel and baseline
include mk/kern.mk

# realprog
include mk/realprog.mk
BUILDINFO_DEFINES := $(shell $(GENBUILDINFO))

# transformation rules

# Compile the c source to assembly
$(BUILD_DIR)/%.s: %.c $(MARKER)
	$(CC) $(CFLAGS) -o $@ -S $<

# Preprocess the assembly source
$(BUILD_DIR)/%.s: %.S $(MARKER)
	$(CPP) $(CPPFLAGS) $(USER_OPTIONS) $(BUILDINFO_DEFINES) -o $@ $<

# Compile assembly source to object code
$(BUILD_DIR)/%.o: %.S $(MARKER)
	$(CPP) -MD $(CPPFLAGS) $(USER_OPTIONS) $(BUILDINFO_DEFINES) -o $(BUILD_DIR)/$*.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.s -a=$(BUILD_DIR)/$*.lst
	$(RM) -f $*.s

$(BUILD_DIR)/%.b: %.s $(MARKER)
	$(AS) $(ASFLAGS) -o $(BUILD_DIR)/$*.o $< -a=$(BUILD_DIR)/$*.lst
	$(LD) $(LDFLAGS) -Ttext 0x0 -s --oformat binary -e begtext -o $@ $(BUILD_DIR)/$*.o

# Compile C source to object code
$(BUILD_DIR)/%.o: %.c $(MARKER)
	$(CC) -MD $(CPPFLAGS) $(USER_OPTIONS) -m32 $(CFLAGS) -o $@ -c $<

#$(BUILD_DIR)/%.16.o: %.16.c $(MARKER)
#	$(CC) -MD $(CPPFLAGS) $(USER_OPTIONS) -m16 -s $(CFLAGS) -o $@ -c $<


#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

IMAGE_BOOTSTRAP := $(BUILD_DIR)/baseline/bootstrap.b
IMAGE_FILES := $(BUILD_DIR)/realprog/prog.b \
               $(BUILD_DIR)/prog.b
IMAGE_SETUP := $(BUILD_DIR)/realprog/prog.b 0x3000 \
               $(BUILD_DIR)/prog.b 0x10000

$(BUILD_DIR)/usb.image: $(IMAGE_BOOTSTRAP) $(IMAGE_FILES) $(BUILDIMAGE) #prog.dis 
	$(BUILDIMAGE) -d usb -o $@ -b $(IMAGE_BOOTSTRAP) $(IMAGE_SETUP)

$(BUILD_DIR)/floppy.image: $(IMAGE_FILES) $(BUILDIMAGE) #prog.dis 
	$(BUILDIMAGE) -d floppy -o $@ -b $(IMAGE_FILES) 0x10000




#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

$(BUILDIMAGE):
	$(MAKE) -C $(BUILDIMAGE_DIR)

export INC_DIR
$(OFFSETS):
	$(MAKE) -C $(OFFSETS_DIR)


#
# Clean and realclean
#
# clean: removes all compiled files, while keeping directory structure
# realclean: same as clean, but removes directories
#
# If you want to clean/realclean a specific project use the target
# project-clean where project is the project you want to clean/realclean
#

CLEAN_TARGETS := $(addsuffix -clean,$(PROJECTS))
REALCLEAN_TARGETS := $(addsuffix -realclean,$(PROJECTS))

$(CLEAN_TARGETS): %-clean:
	find $(BUILD_DIR)/$* -type f -delete

$(REALCLEAN_TARGETS): %-realclean:
	rm -r -f $(BUILD_DIR)/$*

clean: $(CLEAN_TARGETS)

realclean: $(REALCLEAN_TARGETS)
	rm -f $(BUILD_DIR)/*

