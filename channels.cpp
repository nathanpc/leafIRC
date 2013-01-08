/**
 *  channels.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "channels.h"
#include "locations.h"
using namespace std;

Channels::Channels() {
    history_dir = string(HOME_DIR) + string(HISTORY_DIR);
}

void Channels::add(string name) {
    list.push_back(name);
}

void Channels::remove(string name) {
    for (size_t i = 0; i < list.size(); i++) {
        if (list.at(i) == name) {
            remove(i);
            break;
        }
    }
}

void Channels::remove(unsigned int index) {
    list.erase(list.begin() + index);
}

void Channels::cache_name(string channel_name, unsigned int index) {
    string file_name = index + "_" + channel_name;
}
