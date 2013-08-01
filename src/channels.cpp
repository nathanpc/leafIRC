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
 * \param is_channel Is it a channel or a private chat?
 */
void Channels::add(string channel, bool is_channel) {
	if (is_channel) {
		list.push_back("#" + channel);
	} else {
		list.push_back(channel);
	}

	// Create a new vector for the users array.
	vector<string> _users;
	users.push_back(_users);

	// Set the new current channel index.
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
    string filename = config.cache_filename(strip_channel_hash(channel), index);
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
    string filename = config.cache_filename(strip_channel_hash(channel), index);
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
    if (strip_channel_hash(list.at(current)) != channel) {
        for (size_t i = 0; i < list.size(); i++) {
            if (list.at(i) == strip_channel_hash(channel)) {
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
 *  Populate the nicks for a channel.
 *
 *  @param index Channel index.
 *  @param user Username.
 */
void Channels::add_user(unsigned int index, string user) {
	// TODO: Check if the user is already in the array before pushing a new one.
	if (find_user(index, user).empty()) {
		users.at(index).push_back(user);
	}
}

/**
 *  Searches for a user name.
 *
 *  @param channel Channel index.
 *  @param user A user name to search for.
 *  @param searching Are you searching using part of a nick instead of matching?
 *  @returns The user name if it exists, otherwise an empty string.
 */
string Channels::find_user(unsigned int channel, string user, bool searching) {
	string _user;

	for (size_t i = 0; i < users.at(channel).size(); i++) {
		string tuser = users.at(channel).at(i);

		if (searching && (tuser.find(user) != string::npos)) {
			_user = tuser;
		} else if (tuser == user) {
			_user = tuser;
		}
	}

	return _user;
}

/**
 *  Searches for a user name.
 *
 *  @param channel Channel index.
 *  @param user A user name to search for.
 *  @returns The user name if it exists, otherwise an empty string.
 */
string Channels::find_user(unsigned int channel, string user) {
	return find_user(channel, user, false);
}

/**
 * Remove a channel from the list based on its name.
 */
void Channels::remove(string channel) {
    for (size_t i = 0; i < list.size(); i++) {
        if (list.at(i) == strip_channel_hash(channel)) {
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
	users.erase(users.begin() + index);
}

/**
 *  Remove the user's level symbol.
 *
 *  @param user Username to be stripped.
 */
string Channels::strip_user_level(const string user) {
	if ((user[0] == '+') || (user[0] == '@') || (user[0] == '&')) {
		return user.substr(1);
	}

	return user;
}

/**
 *  Remove the channel hash symbol.
 *
 *  @param channel A channel name.
 */
string Channels::strip_channel_hash(const string channel) {
	string chat = channel;
	if (chat.find("#") == 0) {
		chat = chat.substr(1);
	}

	return chat;
}
