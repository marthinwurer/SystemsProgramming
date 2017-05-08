
LIBC_CFLAGS   := $(CFLAGS)
LIBC_CPPFLAGS := $(CPPFLAGS) -D__LIBC
LIBK_CFLAGS   := $(CFLAGS)
LIBK_CPPFLAGS := $(CPPFLAGS) -D__LIBK


LIBC_OBJS = string/memcmp.o \
            string/memcpy.o \
            string/memmove.o \
            string/memset.o \
            string/strlen.o \
            string/strcpy.o \
			string/strpos.o \
			string/atoi.o

LIBC_OBJS := $(addprefix $(BUILD_DIR)/libc/,$(LIBC_OBJS))
LIBK_OBJS := $(LIBC_OBJS:.o=.libk.o)

LIBK = $(BUILD_DIR)/libk.a

$(LIBK): $(LIBK_OBJS) $(MARKER)
	$(AR) rcs $@ $(LIBK_OBJS)


$(BUILD_DIR)/libc/%.o: libc/%.c $(MARKER)
	$(CC) -MD $(LIBC_CPPFLAGS) -m32 $(LIBC_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/libc/%.libk.o: libc/%.c $(MARKER)
	$(CC) -MD $(LIBK_CPPFLAGS) -m32 $(LIBK_CFLAGS) -o $@ -c $<

-include $(LIBC_OBJS:.o=.d)
