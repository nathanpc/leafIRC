/**
 *  config.h
 *
 *  @author Nathan Campos
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

class Config {
    private:
        std::string config_dir;
        std::string history_dir;

        std::string expand_path(const char *directory);
        bool directory_exists(const char *directory);
        void build_dir(const char *directory);

    public:
        // Constructor.
        Config();

        void check_dirs(std::string optional_dir);
        std::string cache_filename(std::string channel_name, unsigned int index);
};

#endif
