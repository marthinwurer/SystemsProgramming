
REALPROG_OBJS := realprog.o

REALPROG_OBJS := $(addprefix $(BUILD_DIR)/realprog/,$(REALPROG_OBJS))



$(BUILD_DIR)/realprog/prog.o: $(REALPROG_OBJS) $(MARKER)
	$(LD_V) $(LDFLAGS) -o $@ -e begtext -Ttext 0x3000 $(REALPROG_OBJS)

$(BUILD_DIR)/realprog/prog.b: $(BUILD_DIR)/realprog/prog.o $(MARKER)
	$(LD_V) $(LDFLAGS) -o $@ -s -e begtext --oformat binary -Ttext 0x3000 $<