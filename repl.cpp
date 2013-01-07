/**
 *  repl.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <stdio.h>

#include "repl.h"
#include "conio/conio.h"
using namespace std;

REPL::REPL() {
    has_started = false;
    string_is_ready = false;
    input_marker = "> ";
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
                // TODO: Go up in the history
            } else if (curr_char == 66) {
                // Down
                // TODO: Go down in the history
            }
        }
    } else if (curr_char != 10) {
        // Others
        char *char_str = (char*)malloc(2 * sizeof(char));
        char_str[0] = curr_char;
        char_str[1] = '\0';

        current_str.append(char_str);
        printf("%c", curr_char);
    } else {
        // Return
        current_str.append("\r\n");
        printf("\n");

        string_is_ready = true;
    }
}

void REPL::eval() {
    
}
