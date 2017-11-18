SRC += pi.cpp
TARGET += $(BUILD)/pi/pi_Os $(BUILD)/pi/pi_O2
LDFLAGS += -lpthread

$(BUILD)/pi/pi_Os: $(BUILD)/pi/pi_Os.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(BUILD)/pi/pi_O2: $(BUILD)/pi/pi_O2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
