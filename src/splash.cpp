/**
 *  splash.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>

#include "splash.h"
#include "color.h"
using namespace std;

void Splash::print() {
    cout << BOLDGREEN << "leafIRC v0.0.1a" << RESET << endl;
    cout << "Developed by" << endl;
    cout << "   \u2022 Nathan Campos <http://about.me/nathanpc>" << endl;
    cout << "   \u2022 Chris Myers <vividexstance@hotmail.com>" << endl;

    cout << endl;
}
