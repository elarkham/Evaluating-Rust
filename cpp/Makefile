# See LICENSE file for copyright and license details.
# Project configuration
include config.mk

TARGET :=
LDFLAGS :=
MODULES := pi
SRC :=

BUILD=build

# Include project modules
include $(patsubst %, %/module.mk, $(MODULES))

OBJ := $(patsubst %.cpp, $(BUILD)/%.o, $(filter %.cpp, $(SRC)))

# Standard build
$(BUILD)/%.o: %.cpp
	@printf "CC [R] $@\n"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@$(CC) $(CFLAGS) -S -o $(@:.o=.s) $<

# Standard targets
all: options $(TARGET)

options: config.mk
	@printf -- "build options:\n"
	@printf -- "CFLAGS  = %s\n" "$(CFLAGS)"
	@printf -- "LDFLAGS = %s\n" "$(LDFLAGS)"
	@printf -- "CC      = %s\n" "$(CC)"

dist: clean

clean:
	@printf "Cleaning\n"
	@rm -rf $(BUILD)

.PHONY: all options check clean dist install uninstall
