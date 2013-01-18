/**
 *  config.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <wordexp.h>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include "inih/cpp/INIReader.h"

#include "config.h"
#include "locations.h"
using namespace std;

Config::Config() {
    config_dir = expand_path(HOME_DIR);
    history_dir = config_dir + string(HISTORY_DIR);

    read_config_file();
    clear_cache();
}

string Config::expand_path(const char *directory) {
    wordexp_t exp_result;
	wordexp(directory, &exp_result, 0);

    return string(exp_result.we_wordv[0]);
}

bool Config::directory_exists(const char *directory) {
    struct stat st;
    return !stat(directory, &st);
}

void Config::build_dir(const char *directory) {
    if (!directory_exists(directory)) {
        int dir = mkdir(directory, S_IRWXU | S_IRGRP);
        if (dir) {
            perror(string("Couldn't create directory " + string(directory)).c_str());
        }
    }
}

void Config::check_dirs(string optional_dir) {
    build_dir(config_dir.c_str());

    if (optional_dir == "history") {
        build_dir(history_dir.c_str());
    }
}

void Config::clear_cache() {
    if (directory_exists(history_dir.c_str())) {
        system(string("rm -r " + history_dir).c_str());
    }
}

bool is_quote(char c) {
    switch(c) {
        case '"':
            return true;
        default:
            return false;
    }
}

string clean_config_string(string str) {
    str.erase(remove_if(str.begin(), str.end(), &is_quote), str.end());
    return str;
}

void Config::read_config_file() {
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

    // Clean the values.
    user_nick = clean_config_string(user_nick);
    user_username = clean_config_string(user_username);
    user_realname = clean_config_string(user_realname);
}

string Config::cache_filename(string channel_name, unsigned int index) {
    ostringstream stream;
    stream << history_dir << "/" << index << "_" << channel_name << ".log";

    return stream.str();
}
