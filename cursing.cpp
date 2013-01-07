/**
 *  cursing.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <curses.h>

#include "cursing.h"
using namespace std;

Curses::Curses() {
    // libcurses setup.
    window = initscr();
    cbreak();
    keypad(stdscr, true);
    noecho();
    getmaxyx(window, rows, cols);
    clear();
    refresh();
}

Curses::~Curses() {
    endwin();
}

void Curses::print(string line) {
    printw(line.c_str());
    refresh();
}
