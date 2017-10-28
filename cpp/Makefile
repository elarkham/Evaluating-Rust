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

OBJ := $(patsubst %.c, $(BUILD)/%.o, $(filter %.c, $(SRC)))

# Standard build
$(BUILD)/%.o: %.c
	@printf "CC [R] $@\n"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

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
