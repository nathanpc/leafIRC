CXX = clang++
CXXFLAGS = -Wall
LDFLAGS = -pthread
OBJ = src/leaf.o src/splash.o src/irc_client.o src/pretty_print_msg.o src/message.o src/config.o src/channels.o src/notification.o src/inih/ini.o src/inih/cpp/INIReader.o src/conio/conio.o
PREFIX = /usr/local

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
	LDFLAGS += -lgrowl
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += $(shell pkg-config --cflags glib-2.0 libnotify)
	LDFLAGS += $(shell pkg-config --libs glib-2.0 libnotify)
endif

all: leaf

leaf: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

debug: CXXFLAGS += -g3 -DDEBUG
debug: leaf

%.o: %.c
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $<

clean:
	rm -rf src/*.o src/core
	rm -rf src/conio/*.o
	rm -rf leaf

install: leaf
	chmod a+x install.sh
	sh ./install.sh $(PREFIX)/bin

uninstall:
	sudo rm -f $(PREFIX)/bin/leaf
	rm -rf ~/.leafirc/
