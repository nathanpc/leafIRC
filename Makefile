CXX = clang++
CXXFLAGS = -Wall -pthread
OBJ = leaf.o irc_client.o message.o config.o channels.o conio/conio.o repl.o

all: leaf

leaf: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

debug: CXXFLAGS += -g3
debug: leaf

%.o: %.c
	$(CXX) $(CXXFLAGS) $<

clean:
	rm -rf *.o core
	rm -rf conio/*.o
	rm -rf leaf
