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
#include <stdlib.h>
#include <wordexp.h>
#include <sstream>

#include "config.h"
#include "locations.h"
using namespace std;

Config::Config() {
    config_dir = expand_path(HOME_DIR);
    history_dir = config_dir + string(HISTORY_DIR);

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

string Config::cache_filename(string channel_name, unsigned int index) {
    ostringstream stream;
    stream << history_dir << "/" << index << "_" << channel_name << ".log";

    return stream.str();
}
