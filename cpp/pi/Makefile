# See LICENSE file for copyright and license details.
# Project configuration
include config.mk

LDFLAGS += 
MODULES :=
LIBS :=
SRC :=

# Include project modules
include $(patsubst %, %/module.mk, $(MODULES))

OBJ := $(patsubst %.c, build/%.o, $(filter %.c, $(SRC)))

# Standard build
build/%.o: %.c
	@printf "CC [R] $@\n"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

# Standard targets
all: options

options: config.mk
	@printf -- "build options:\n"
	@printf -- "CFLAGS  = %s\n" "$(CFLAGS)"
	@printf -- "LDFLAGS = %s\n" "$(LDFLAGS)"
	@printf -- "CC      = %s\n" "$(CC)"
	@echo $(OBJ_DEBUG)

dist: clean

clean:
	@printf "Cleaning\n"
	@rm -rf build

.PHONY: all options check clean dist install uninstall
