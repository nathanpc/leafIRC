CXX = clang++
CXXFLAGS = -Wall
LDFLAGS = -pthread
OBJ = src/leaf.o src/splash.o src/irc_client.o src/pretty_print_msg.o src/message.o src/config.o src/channels.o src/notification.o src/inih/ini.o src/inih/cpp/INIReader.o src/conio/conio.o
PREFIX = /usr/bin

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
	LDFLAGS += -lgrowl
endif
ifeq ($(UNAME), Linux)
    PACKAGES = libnotify

	CXXFLAGS += $(shell pkg-config --cflags $(PACKAGES))
	LDFLAGS += $(shell pkg-config --libs-only-L --libs-only-other $(PACKAGES))
	LIBS = $(shell pkg-config --libs-only-l $(PACKAGES))
endif

all: leaf

leaf: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

debug: CXXFLAGS += -g3 -DDEBUG
debug: leaf

%.o: %.c
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $<

clean:
	rm -rf src/*.o src/core
	rm -rf src/conio/*.o
	rm -rf src/inih/*.o
	rm -rf src/inih/cpp/*.o
	rm -rf leaf

install: leaf
	sudo chmod a+x ./leaf
	sudo install ./leaf $(PREFIX)

uninstall:
	sudo rm -f $(PREFIX)/bin/leaf
	rm -rf ~/.leafirc/
