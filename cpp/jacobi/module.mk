SRC += jacobi.cpp
TARGET += $(BUILD)/jacobi/jacobi_Os $(BUILD)/jacobi/jacobi_O2
LDFLAGS += -lpthread

$(BUILD)/jacobi/jacobi_Os: $(BUILD)/jacobi/jacobi_Os.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(BUILD)/jacobi/jacobi_O2: $(BUILD)/jacobi/jacobi_O2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
