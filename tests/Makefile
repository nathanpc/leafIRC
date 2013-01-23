CXX = clang++
CXXFLAGS = -Wall -pthread

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = ./googletest
SRC_DIR = ../src

# Flags passed to the preprocessor.
CPPFLAGS += -I $(GTEST_DIR)/include -I $(SRC_DIR)

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra

LIB  = $(GTEST_DIR)/make/gtest_main.a
SRC  = $(NAME)_test.cpp
OBJS = $(SRC_DIR)/irc_client.cpp $(SRC_DIR)/repl.cpp $(SRC_DIR)/channels.cpp \
	$(SRC_DIR)/message.cpp $(SRC_DIR)/pretty_print_msg.cpp \
	$(SRC_DIR)/conio/conio.cpp $(SRC_DIR)/inih/cpp/INIReader.cpp \
	$(SRC_DIR)/inih/ini.cpp $(SRC_DIR)/config.cpp

tests: $(NAME)_test

$(NAME)_test: $(SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LIB) $(OBJS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm -rf $(NAME)_test
	rm -rf *.dSYM
