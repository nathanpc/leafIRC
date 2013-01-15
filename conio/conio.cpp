// Credits: http://stackoverflow.com/a/7469410/126353

#include <termios.h>
#include <cstdio>

#include "conio.h"
using namespace std;

static struct termios old, _new;

/* Initialize _new terminal i/o settings */
void Conio::initTermios(int echo) {
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  _new = old; /* make _new settings same as old settings */
  _new.c_lflag &= ~ICANON; /* disable buffered i/o */
  _new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &_new); /* use these _new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void Conio::resetTermios() {
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char Conio::getch_(int echo) {
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char Conio::getch() {
  return getch_(0);
}

/* Read 1 character with echo */
char Conio::getche() {
  return getch_(1);
}
