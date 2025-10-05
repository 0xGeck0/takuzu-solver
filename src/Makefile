# ===== src/Makefile =====
# Compilation parameters
CC       = gcc
CFLAGS   = -std=c11 -Wall -Wextra
CPPFLAGS = -I../include
LDFLAGS  = 

# Object files
OBJS = grid.o grid_io.o takuzu.o

# Binary to generate (at the project root)
BIN = ../takuzu

# Declare phony targets (not real files)
.PHONY: all clean test help

# 1. all: compile all source files and generate takuzu binary
all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

# 2. clean: remove temporary files and binary
clean:
	rm -f $(OBJS) $(BIN)

# 3. test: run your own tests (example)
test: all
	@echo "Running takuzu test..."
	@$(BIN) -h || true

# 4. help: show available targets
help:
	@echo "Available targets:"
	@echo "  all   : compile source files and generate takuzu binary"
	@echo "  clean : remove object files and binary"
	@echo "  test  : build and run a quick test"
	@echo "  help  : display this help message"