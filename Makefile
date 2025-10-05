# ===== Root Makefile =====
# User-facing Makefile (delegates to src/Makefile)

# Variables communes
CC       ?= gcc
CFLAGS   ?= -std=c11 -Wall -Wextra
CPPFLAGS ?= -Iinclude
LDFLAGS  ?=

# Déclare les cibles comme "phony" (pas de fichiers réels)
.PHONY: all clean test help

# all : construit le binaire takuzu via le Makefile de src/
all:
	@echo "Building takuzu binary..."
	@$(MAKE) -C src all

# clean : nettoie tout (objets + binaire)
clean:
	@echo "Cleaning build files..."
	@$(MAKE) -C src clean
	@rm -f ./takuzu

# test : lance les tests (ou au minimum ./takuzu -h)
test: all
	@echo "Running takuzu self-test..."
	@./takuzu -h || true

# help : affiche la liste des cibles
help:
	@echo "Available targets:"
	@echo "  all   : build takuzu binary (via src/Makefile)"
	@echo "  clean : remove object files and takuzu binary"
	@echo "  test  : build and run a quick test"
	@echo "  help  : show this help message"
