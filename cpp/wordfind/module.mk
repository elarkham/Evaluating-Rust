SRC += hash.cpp wordfind.cpp
TARGET += $(BUILD)/wordfind/wordfind_Os $(BUILD)/wordfind/wordfind_O2
LDFLAGS += -lpthread

wordfind: $(BUILD)/wordfind/wordfind_Os $(BUILD)/wordfind/wordfind_O2

$(BUILD)/wordfind/wordfind_Os: $(BUILD)/wordfind/hash_Os.o $(BUILD)/wordfind/wordfind_Os.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/wordfind/wordfind_O2: $(BUILD)/wordfind/hash_O2.o $(BUILD)/wordfind/wordfind_O2.o
	$(CC) $(LDFLAGS) -o $@ $^
