/**
 *  repl.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <stdlib.h>

#include "repl.h"
using namespace std;

REPL::REPL() {
    has_started = false;
    input_marker = "> ";
}

void REPL::rewrite() {
    cout << input_marker + current_str << flush;
}

string REPL::read() {
    has_started = true;
    current_str = "";
    char curr_char;

    cout << "> ";
    //printf("%d\n", curr_char = getchar());

    while (true) {
        curr_char = getchar();

        if (curr_char != 10) {
            char *char_str = (char*)malloc(2 * sizeof(char));
            char_str[0] = curr_char;
            char_str[1] = '\0';

            current_str.append(char_str);
        } else {
            break;
        }
    }

    return current_str + "\r\n";
}

void REPL::eval() {
    
}
