VERSION := 0.0.1

# System paths
PREFIX := /usr/local
BINPREFIX := $(PREFIX)/bin
INCLUDEPREFIX := $(PREFIX)/include
LIBPREFIX := $(PREFIX)/lib
MANPREFIX := $(PREFIX)/man

# Linking flags
LDFLAGS :=

# C Compiler settings
CC := cc 
CFLAGS := -std=gnu99 -pedantic -O0 -Wall -Wextra
