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
		std::string strip_channel_hash(const std::string channel);

    public:
        std::vector<std::string> list;
        int current;
        Config config;

        // Constructor.
        Channels();

        void add(std::string channel, bool is_channel = true);
        void cache(std::string channel, std::string line);
        std::string load_cache(std::string channel);
        unsigned int find_index(std::string channel);
        void remove(std::string channel);
        void remove(unsigned int index);
};

#endif	// LEAF_CHANNEL_H_
