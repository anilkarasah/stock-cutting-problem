# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build

# Find all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Create a list of object files by replacing .c with .o
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Output executable name
TARGET = main

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Rule to build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/types.h $(SRC_DIR)/corner.h $(SRC_DIR)/item.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
