SRC += vector.cpp
TARGET += $(BUILD)/vector/vector_O0 $(BUILD)/vector/vector_O2
LDFLAGS += -lpthread

vector: $(BUILD)/vector/vector_O0 $(BUILD)/vector/vector_O2

$(BUILD)/vector/vector_O0: $(BUILD)/vector/vector_O0.o
	@$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/vector/vector_O2: $(BUILD)/vector/vector_O2.o
	@$(CC) $(LDFLAGS) -o $@ $^
