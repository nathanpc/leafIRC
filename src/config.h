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
        void clear_cache();
        void read_general_config_file();
        void read_user_config_file(const char *server_alias);

    public:
        std::string server_location;
        std::string user_nick;
        std::string user_username;
        std::string user_realname;
        std::string user_password;

        // Constructor.
        Config();

        void check_dirs(std::string optional_dir);
        std::string cache_filename(std::string channel_name, unsigned int index);
        void load_user_config(const char *server_alias);
};

#endif
