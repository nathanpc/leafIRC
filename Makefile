CXX = clang++
CXXFLAGS = -Wall -pthread
OBJ = src/leaf.o src/splash.o src/irc_client.o src/pretty_print_msg.o src/message.o src/config.o src/inih/ini.o src/inih/cpp/INIReader.o src/channels.o src/conio/conio.o src/repl.o

all: leaf

leaf: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

debug: CXXFLAGS += -g3 -DDEBUG
debug: leaf

%.o: %.c
	$(CXX) $(CXXFLAGS) $<

clean:
	rm -rf src/*.o src/core
	rm -rf src/conio/*.o
	rm -rf leaf

install:
	chmod a+x install.sh
	sh ./install.sh

uninstall:
	sudo rm -f /usr/bin/leaf
	rm -rf ~/.leafirc/
