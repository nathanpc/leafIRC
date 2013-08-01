/**
 *  channels.h
 *
 *  @author Nathan Campos
 */

#ifndef LEAF_CHANNEL_H_
#define LEAF_CHANNEL_H_

#include <string>
#include <vector>
#include "config.h"

class Channels {
	private:
		static std::string strip_channel_hash(const std::string channel);

    public:
        std::vector<std::string> list;
		std::vector<std::vector<std::string> > users;
        int current;
        Config config;

        // Constructor.
        Channels();

		// Modifiers.
        void add(std::string channel, bool is_channel = true);
        void remove(std::string channel);
        void remove(unsigned int index);

		// Users.
		void add_user(unsigned int index, std::string user);
		void remove_user(unsigned int index, std::string user);
		std::string find_user(unsigned int channel, std::string user);
		std::string find_user(unsigned int channel, std::string user, bool searching);

		// Helpers.
		void cache(std::string channel, std::string line);
        std::string load_cache(std::string channel);
        unsigned int find_index(std::string channel);
		static std::string strip_user_level(const std::string user);
};

#endif	// LEAF_CHANNEL_H_
