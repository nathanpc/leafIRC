/**
 *  channels.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "channels.h"
#include "config.h"
using namespace std;

void Channels::add(string channel) {
    list.push_back(channel);
    current = list.size() - 1;
}

void Channels::cache(string channel, string line) {
    unsigned int index = find_index(channel);
    string filename = config.cache_filename(channel, index);
    ofstream file;


    config.check_dirs("history");
    file.open(filename.c_str(), ios::out | ios::app);

    if (file.is_open()) {
        file << line;
        file.close();
    }
}

string Channels::load_cache(string channel) {
    unsigned int index = find_index(channel);
    string filename = config.cache_filename(channel, index);
    ifstream file;
    string content;

    config.check_dirs("history");
    file.open(filename.c_str(), ios::in);

    if (file.is_open()) {
        content.assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));        
        file.close();
    }

    return content;
}

unsigned int Channels::find_index(string channel) {
    if (list.at(current) != channel) {
        for (size_t i = 0; i < list.size(); i++) {
            if (list.at(i) == channel) {
                return i;
                break;
            }
        }
    } else {
        return current;
    }

    return NULL;
}

void Channels::remove(string channel) {
    for (size_t i = 0; i < list.size(); i++) {
        if (list.at(i) == channel) {
            remove(i);
            break;
        }
    }
}

void Channels::remove(unsigned int index) {
    list.erase(list.begin() + index);
}
