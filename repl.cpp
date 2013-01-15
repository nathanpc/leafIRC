/**
 *  repl.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "repl.h"
#include "conio/conio.h"
#include "color.h"
using namespace std;

REPL::REPL() {
    has_started = false;
    string_is_ready = false;
    input_marker = string(BOLDCYAN) + ":: " + string(RESET);
    history_current_position = 0;
}

REPL::~REPL() {
    clear();
    Conio::initTermios(1);
    Conio::resetTermios();
    system("stty echo cooked");
}

void REPL::add_history() {
    if (history.size() >= 100) {
        history.erase(history.begin());
    }

    history.push_back(current_str);
}

void REPL::clear() {
    int curr_input_length = current_str.length() + input_marker.length();

    for (int i = 0; i < curr_input_length; i++) {
        printf("\b \b");
    }
}

void REPL::rewrite() {
    cout << input_marker + current_str << flush;
}

void REPL::read() {
    has_started = true;
    char curr_char;

    if (string_is_ready) {
        string_is_ready = false;
        current_str = "";
        external_command.clear();

        cout << input_marker << flush;
    }

    //printf("%d\n", curr_char = Conio::getche());
    curr_char = Conio::getch();
    
    if (curr_char == 3) {
        // Control + C
        exit(0);
    } else if (curr_char == 127) {
        // Backspace
        if (current_str != "") {
            current_str = current_str.substr(0, current_str.length() - 1);
            printf("\b \b");
        }
    } else if (curr_char == 27) {
        if (Conio::getche() == 91) {
            // Arrow keys
            curr_char = Conio::getche();
            if (curr_char == 65) {
                // Up
                if (history_current_position < history.size()) {
                    clear();

                    current_str = history.at(history.size() - 1 - history_current_position);
                    history_current_position++;

                    rewrite();
                }
            } else if (curr_char == 66) {
                // Down
                if (history_current_position != 0) {
                    clear();

                    if (history_current_position - 1 > 0) {
                        current_str = history.at(history.size() + 1 - history_current_position);
                        history_current_position--;
                    } else {
                        history_current_position = 0;
                        current_str = "";
                    }

                    rewrite();
                }
            }
        }
    } else if (curr_char != 10) {
        // Others
        char *char_str = new char[2];
        char_str[0] = curr_char;
        char_str[1] = '\0';

        current_str.append(char_str);
        printf("%c", curr_char);

        delete [] char_str;
    } else {
        // Return
        if (current_str != "") {
            add_history();
            history_current_position = 0;

            if (eval()) {
                current_str.append("\r\n");
            }

            printf("\n");
            string_is_ready = true;
        }
    }
}

bool REPL::eval() {
    if (current_str != "" && current_str.at(0) == '/') {
        // Command
        string command = current_str.substr(1, current_str.find(" ") - 1);

        if (command == "switch") {
            // Switch the current channel.
            string switch_channel = current_str.substr(current_str.find(" ") + 1);

            if (switch_channel.at(0) == '#') {
                switch_channel = switch_channel.substr(1);
            }

            external_command.push_back("switch");
            external_command.push_back(switch_channel);
        } else {
            // Common IRC command.
            current_str = current_str.substr(1);
            return true;
        }

        return false;
    } else {
        // Message
        external_command.push_back("message");

        return true;
    }
}
