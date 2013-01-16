CXX = clang++
CXXFLAGS = -Wall -pthread
OBJ = leaf.o splash.o irc_client.o pretty_print_msg.o message.o config.o inih/ini.o inih/cpp/INIReader.o channels.o conio/conio.o repl.o

all: leaf

leaf: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

debug: CXXFLAGS += -g3 -DDEBUG
debug: leaf

%.o: %.c
	$(CXX) $(CXXFLAGS) $<

clean:
	rm -rf *.o core
	rm -rf conio/*.o
	rm -rf leaf
