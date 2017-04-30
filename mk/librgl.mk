
LIBRGL_CFLAGS := $(CFLAGS)
LIBRGL_CPPFLAGS := $(CPPFLAGS)

LIBRGL_K_CFLAGS := $(CFLAGS)
LIBRGL_K_CPPFLAGS := $(CPPFLAGS) -D__KERNEL


LIBRGL_OBJS =


LIBRGL_OBJS := $(addprefix ($BUILD_DIR)/librgl/,$(LIBRGL_OBJS))
LIBRGL_K_OBJS := $(LIBRGL_OBJS:.o=.ko)


$(LIBRGLK) = $(BUILD_DIR)/librglk.a

$(LIBRGLK): $(LIBRGL_K_OBJS) $(MARKER)
	$(AR) rcs $@ $(LIBRGL_K_OBJS)


$(BUILD_DIR)/librgl/%.o: librgl/%.c $(MARKER)
	$(CC) -MD $(LIBRGL_CPPFLAGS) -m32 $(LIBRGL_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/librgl/%.ko: librgl/%.c $(MARKER)
	$(CC) -MD $(LIBRGL_K_CPPFLAGS) -m32 $(LIBRGL_K_CFLAGS) -o $@ -c $<

-include $(LIBRGL_OBJS:.o=.d)