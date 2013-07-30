/**
 *  config.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <wordexp.h>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include "inih/cpp/INIReader.h"

#include "config.h"
#include "locations.h"
#include "color.h"
using namespace std;

/**
 * Constructor for the Config.
 */
Config::Config() {
	Config(false);
}

/**
 * Constructor for the Config.
 *
 * \param load_config Want to load the configuration files or just get some variables?
 */
Config::Config(bool load_config) {
    config_dir = expand_path(HOME_DIR);
    history_dir = config_dir + string(HISTORY_DIR);

	if (load_config) {
	    read_general_config_file();
		clear_cache();
    }
}

/**
 * Gets the absolute path from a relative path (expand it).
 *
 * \param directory Relative path.
 * \return Absolute path.
 */
string Config::expand_path(const char *directory) {
    wordexp_t exp_result;
	wordexp(directory, &exp_result, 0);

    return string(exp_result.we_wordv[0]);
}

/**
 * Check if a directory exists.
 *
 * \param directory A directory.
 * \return true if the directory exists.
 */
bool Config::directory_exists(const char *directory) {
    struct stat st;
    return !stat(directory, &st);
}

/**
 * Make a directory if it doesn't exist.
 *
 * \param directory A directory.
 */
void Config::build_dir(const char *directory) {
    if (!directory_exists(directory)) {
        int dir = mkdir(directory, S_IRWXU | S_IRGRP);
        if (dir) {
            perror(string("Couldn't create directory " + string(directory)).c_str());
        }
    }
}

/**
 * Check if all the required directories exist.
 *
 * \param optional_dir A secondary directory to check for.
 */
void Config::check_dirs(string optional_dir) {
    build_dir(config_dir.c_str());

    if (optional_dir == "history") {
        build_dir(history_dir.c_str());
    }
}

/**
 * Remove the cache directory.
 */
void Config::clear_cache() {
    if (directory_exists(history_dir.c_str())) {
        system(string("rm -r " + history_dir).c_str());
    }
}

/**
 * Check if a character is a quote. (used for string::erase)
 *
 * \see clean_config_string
 * \param c Character to be checked.
 * \return true if the character is a quote.
 */
bool is_quote(char c) {
    switch(c) {
        case '"':
            return true;
        default:
            return false;
    }
}

/**
 * Remove any quotes from a configuration string.
 *
 * \param str String to be cleaned.
 * \return Clean string.
 */
string clean_config_string(string str) {
    str.erase(remove_if(str.begin(), str.end(), &is_quote), str.end());
    return str;
}

/**
 * Read the general config file. (leaf.conf)
 */
void Config::read_general_config_file() {
    string config_file = config_dir + "/leaf.conf";
    INIReader reader = INIReader(config_file);
    if (reader.ParseError() < 0) {
        cout << "Can't load configuration file at \"" << config_file << "\"" << endl;
        exit(1);
    }

    // Get the values from the config file.
    user_nick = reader.Get("user", "nick", "leafirc");
    user_username = reader.Get("user", "username", "leafirc");
    user_realname = reader.Get("user", "realname", "leafIRC");
    user_password = reader.Get("user", "nickserv", "");

    // Clean the values.
    user_nick = clean_config_string(user_nick);
    user_username = clean_config_string(user_username);
    user_realname = clean_config_string(user_realname);
    user_password = clean_config_string(user_password);
}

/**
 * Save the configuration to the file.
 *
 * \param file_name Name of the configuration with out the extension.
 * \param contents The contents that will be saved to the file.
 */
void Config::save_config(string file_name, string contents) {
	string location = config_dir + "/" + file_name + ".conf";

	ofstream config_file;
	config_file.open(location.c_str(), ios::out | ios::trunc);
	config_file << contents;
	config_file.close();
}

/**
 * Load the user configuration based on the user's alias.
 *
 * \see read_user_config_file
 * \param server_alias Server alias.
 */
void Config::load_user_config(const char *server_alias) {
    string config_file = config_dir + "/user.conf";
    INIReader reader = INIReader(config_file);
    string alias = string(server_alias);
    
    if (reader.ParseError() >= 0) {
        // Get the server location based on the alias given.
        server_location = reader.Get(alias, "location", "");
        server_location = clean_config_string(server_location);

        if (server_location != "") {
            user_nick = reader.Get(alias, "nick", user_nick);
            user_username = reader.Get(alias, "username", user_username);
            user_realname = reader.Get(alias, "realname", user_realname);
            user_password = reader.Get(alias, "nickserv", user_password);
            
            // Clean the values.
            user_nick = clean_config_string(user_nick);
            user_username = clean_config_string(user_username);
            user_realname = clean_config_string(user_realname);
            user_password = clean_config_string(user_password);
            
            // Populate the channels vector.
            populate_channels_vector(clean_config_string(
                reader.Get(alias, "channels", "")));
        } else {
            server_location = string(alias);
        }
    } else {
		// TODO: Error?
        server_location = string(alias);
    }
}

/**
 * Builds the cache filename.
 *
 * \param channel_name Channel to cache.
 * \param index Channel's index.
 * \return Cache filename.
 */
string Config::cache_filename(string channel_name, unsigned int index) {
    ostringstream stream;
    stream << history_dir << "/" << index << "_" << channel_name << ".log";

    return stream.str();
}

/**
 * Populate the channels vector with the string from the config file.
 *
 * \param channels_str String from the configuration file.
 */
void Config::populate_channels_vector(string channels_str) {
    while (channels_str.find(",") != string::npos) {
        size_t pos = channels_str.find(",");

        channels.push_back(channels_str.substr(0, pos));
        channels_str.erase(0, pos + 1);
    }
    
    if (!channels_str.empty()) {
        channels.push_back(channels_str);
    }
}


// Config_Set stuff.

/**
 * Config_Set constructor
 */
Config_Set::Config_Set() {
}

/**
 * Create the interactive dialog to get the user's information.
 */
void Config_Set::user() {
	cout << BOLDWHITE << "Setting up the user credentials" << RESET << endl;

	cout << "Nickname: ";
	getline(cin, nick);
	cout << "Username: ";
	getline(cin, username);
	cout << "Real Name: ";
	getline(cin, realname);
}

/**
 * Save the configuration to the file.
 */
void Config_Set::save(string name) {
	Config config(false);
	string ini;
	if (name.empty()) {
		// Save everything!
	} else if (name == "user") {
		ini = "[user]\nnick=\"" + nick + "\"\nusername=\"" + username + "\"\nrealname=\"" + realname + "\"\n";
		config.save_config("leaf", ini);
	} else if (name == "networks") {
		// TODO: Everything?
	} else {
		cerr << "Couldn't understand the name given to Config_Set::save" << endl;
		exit(EXIT_FAILURE);
	}

	cout << BOLDGREEN << "Saved!" << RESET << endl;
}
