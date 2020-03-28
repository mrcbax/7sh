CFLAGS = -Wall -pthread
CC = clang
BUILD_DIR = ./build
SRC_DIR = ./src

run: build _run clean

_run:
	@$(BUILD_DIR)/7sh

all: build

clean:
	@rm -rf $(BUILD_DIR)/*

build: 7sh

default: build

# $(CC) $(SRC_DIR) -o $(BUILD_DIR) $(CFLAGS)

7sh:
	pushd $(SRC_DIR); $(CC) main.c builtin.c -o ../$(BUILD_DIR)/7sh $(CFLAGS); popd
