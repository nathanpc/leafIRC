CC=g++
CPPFLAGS=-Wall
OBJ=leaf.o irc_client.o message.o repl.o

all: leaf

leaf: $(OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $<

clean:
	rm -rf *.o leaf
