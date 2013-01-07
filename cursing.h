/**
 *  cursing.h
 *
 *  @author Nathan Campos
 */

#ifndef CURSING_H_
#define CURSING_H_

#include <string>
#include <curses.h>

class Curses {
    public:
        WINDOW *window;
        int rows;
        int cols;

        Curses();
        ~Curses();

        void print(std::string line);
};

#endif
