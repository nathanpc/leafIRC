CXX = clang++
CXXFLAGS = -Wall
OBJ = leaf.o irc_client.o message.o config.o channels.o conio/conio.o repl.o

all: leaf

leaf: $(OBJ)
	$(CXX) $^ -o $@

debug: CXXFLAGS += -g3
debug: leaf

%.o: %.c
	$(CXX) $(CXXFLAGS) $<

clean:
	rm -rf *.o
	rm -rf conio/*.o
	rm -rf leaf
