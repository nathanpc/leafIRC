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

		void populate_channels_vector(std::string channels_str);

    public:
        std::string server_location;
        std::string user_nick;
        std::string user_username;
        std::string user_realname;
        std::string user_password;
        std::vector<std::string> channels;

        // Constructor.
        Config();
        Config(bool load_config);

        void check_dirs(std::string optional_dir);
        std::string cache_filename(std::string channel_name, unsigned int index);

        void load_user_config(const char *server_alias);
        void save_config(std::string file_name, std::string contents);
};

class Config_Set {
	private:
		std::string nick;
		std::string username;
		std::string realname;

	public:
		Config_Set();

		void user();

		void save(std::string name = "");
};

#endif
