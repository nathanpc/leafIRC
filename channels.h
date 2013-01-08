/**
 *  channels.h
 *
 *  @author Nathan Campos
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <string>
#include <vector>

class Channels {
    private:
        std::string history_dir;

        void cache_name(std::string channel_name, unsigned int index);

    public:
        std::vector<std::string> list;

        // Constructor.
        Channels();

        void add(std::string name);
        void remove(std::string name);
        void remove(unsigned int index);
};

#endif
