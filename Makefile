# ===== Root Makefile =====
# User-facing Makefile (delegates to src/Makefile)

# Commons variables
CC       ?= gcc
CFLAGS   ?= -std=c11 -Wall -Wextra
CPPFLAGS ?= -Iinclude

# Declare targets as "phony"
.PHONY: all clean test help

# all : builds the takuzu binary via the src/Makefile
all:
	@echo "Building takuzu binary..."
	@$(MAKE) -C src all

# clean : removes all build artifacts (object files + binary)
clean:
	@echo "Cleaning build files..."
	@$(MAKE) -C src clean
	@rm -f ./takuzu

# test : runs a quick 	test
test: all
	@echo "Running takuzu self-test..."
	@./takuzu -h || true

# help : displays available targets
help:
	@echo "Available targets:"
	@echo "  all   : build takuzu binary (via src/Makefile)"
	@echo "  clean : remove object files and takuzu binary"
	@echo "  test  : build and run a quick test"
	@echo "  help  : show this help message"
