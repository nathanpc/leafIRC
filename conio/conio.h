// Credits: http://stackoverflow.com/a/7469410/126353

#ifndef CONIO_H_
#define CONIO_H_

class Conio {
    private:
        static char getch_(int echo);
    public:
        static void resetTermios();
        static void initTermios(int echo);

        static char getch();
        static char getche();
};

#endif
