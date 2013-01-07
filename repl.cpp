/**
 *  repl.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "repl.h"
using namespace std;

REPL::REPL() {
    //system("stty raw -echo");
}

/*REPL::~REPL() {
    system("stty cooked");
}*/

int kbhit (void)
{
  struct timeval tv;
  fd_set rdfs;
 
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
 
}
void REPL::getchr() {
    if (kbhit()) {
        char c = getchar();
        if (c != 'q') {
            cout << c << endl;
        } else {
            system("stty cooked echo");
        }
    }
}
