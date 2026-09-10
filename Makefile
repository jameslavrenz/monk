CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -Wpedantic
LDFLAGS =

TARGET  = lexer
SRCS    = lexer.c main.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run rebuild

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
