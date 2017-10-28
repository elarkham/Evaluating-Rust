SRC += pi.c
TARGET += $(BUILD)/pi/pi
LDFLAGS += -lpthread

$(BUILD)/pi/pi: $(BUILD)/pi/pi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
