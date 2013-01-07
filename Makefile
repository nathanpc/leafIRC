CC=g++
CPPFLAGS=-Wall
OBJ=leaf.o irc_client.o message.o conio/conio.o repl.o

all: leaf

leaf: $(OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $<

clean:
	rm -rf *.o
	rm -rf conio/*.o
	rm -rf leaf
