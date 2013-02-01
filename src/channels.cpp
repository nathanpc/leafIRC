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

/**
 * Channels constructor.
 */
Channels::Channels() {
    current = -1;
}

/**
 * Add a channel to the list.
 *
 * \param channel Channel to be added.
 */
void Channels::add(string channel) {
    list.push_back(channel);
    current = list.size() - 1;
}

/**
 * Cache a message from a channel.
 *
 * \param channel Channel which the message came from.
 * \param line The message to be cached.
 */
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

/**
 * Load the cache for a channel.
 *
 * \param channel Channel to get the cache.
 * \return The cache.
 */
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

/**
 * Find the index in the list of channels by the channel name.
 *
 * \param channel Channel to search for.
 * \return Index if the channel was found, otherwise NULL.
 */
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

/**
 * Remove a channel from the list based on its name.
 */
void Channels::remove(string channel) {
    for (size_t i = 0; i < list.size(); i++) {
        if (list.at(i) == channel) {
            remove(i);
            break;
        }
    }
}

/**
 * Remove a channel from the list based on its index.
 */
void Channels::remove(unsigned int index) {
    list.erase(list.begin() + index);
}
