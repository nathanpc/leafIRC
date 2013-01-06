CC=g++
CFLAGS=-Wall -g3
OBJ=leaf.o irc_client.o

all: leaf

leaf: $(OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o leaf
