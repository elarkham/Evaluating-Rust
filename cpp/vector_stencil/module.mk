SRC += vector_stencil.cpp
TARGET += $(BUILD)/vector_stencil/vector_stencil_O0 $(BUILD)/vector_stencil/vector_stencil_O2
LDFLAGS += -lpthread

vector_stencil: $(BUILD)/vector_stencil/vector_stencil_O0 $(BUILD)/vector_stencil/vector_stencil_O2

$(BUILD)/vector_stencil/vector_stencil_O0: $(BUILD)/vector_stencil/vector_stencil_O0.o
	@$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/vector_stencil/vector_stencil_O2: $(BUILD)/vector_stencil/vector_stencil_O2.o
	@$(CC) $(LDFLAGS) -o $@ $^
