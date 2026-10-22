CC      = cc
CFLAGS  = -std=c23 -Wall -Wextra -Wpedantic
LDFLAGS =
SANITIZE = -fsanitize=address,undefined

TARGET  = monk
SRCS    = lexer.c main.c
OBJS    = $(SRCS:.c=.o)
TEST_TARGET = $(TARGET)-sanitize

.PHONY: all clean run rebuild test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test:
	$(CC) $(CFLAGS) $(SANITIZE) -o $(TEST_TARGET) $(SRCS)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_TARGET)

rebuild: clean all
