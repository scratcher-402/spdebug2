CC = gcc
CFLAGS = -O2 -fPIC
LDFLAGS = -shared
TARGET = libspdebug.so
EDITOR = micro

OBJ_DIR = obj

SRCS = print.c debug.c
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean edit

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

edit:
	micro $(wildcard *.c) $(wildcard *.h) Makefile
