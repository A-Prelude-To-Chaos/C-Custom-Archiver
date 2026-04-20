CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

OBJS = main.o archive.o bitio.o huffman.o
TARGET = carch

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)