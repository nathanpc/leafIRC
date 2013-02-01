/**
 *  pretty_print_msg.cpp
 *
 *  @author Nathan Campos
 */

#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "channels.h"
#include "color.h"
#include "irc_reply_codes.h"
#include "message.h"
#include "pretty_print_msg.h"
using namespace std;

/**
 * Constructor for the class.
 *
 * \param _buffer Message string.
 */
Pretty_Print_Message::Pretty_Print_Message(const char *_buffer) {
    buffer = _buffer;
    echo = true;
}

/**
 * Create a color string based on the string's letters.
 *
 * \param nickname Nickname to be colorized.
 * \param include_msg Include the whole message with it.
 * \return Colorized string.
 */
string Pretty_Print_Message::color_string(string nickname, bool include_msg) {
    // Generate a color for the nick based on its letters.
    string chars = " abcdefghijklmnopqrstuvwxyz1234567890_-";
    int color = 0;
    char color_chr[2];

    for (size_t i = 0; i < nickname.size(); i++) {
        // Get the characted position in our dictionary.
        size_t pos = chars.find(nickname.at(i));

        // If we couldn't find it just default it to 10.
        if (pos == string::npos) {
            pos = 10;
        }
        
        color += pos;
    }

    // Build the string.
    color = (color % 7) + 1;
    sprintf(color_chr, "%d", color);
    
    if (!include_msg) {
        return "\033[1m\033[3" + string(color_chr) + "m";
    } else {
        return "\033[1m\033[3" + string(color_chr) + "m" + nickname + RESET;
    }
}

/**
 * Generate the pretty formatted string based on the message.
 *
 * \param message Pointer to a Message class.
 * \param channels Pointer to a Channels class.
 * \return Prettyfied string.
 */
string Pretty_Print_Message::generate(Message &message, Channels &channels) {
    // Parse and return a better and more human-readable message.
    string str_buffer(buffer);
    vector<string> arguments = message.get_command_args();

    if (message.get_command() == "PRIVMSG") {
        if (arguments.at(0).at(0) == '#') {
            // Channel message.
            if (arguments.at(0) != "#" + channels.list.at(channels.current)) {
                echo = false;
            }

            // A normal message.
            string nickname = message.get_nickname();
            str_buffer = color_string(nickname, false) + "<" + nickname + "> " + string(RESET) + arguments.at(1) + "\r\n";

            if (arguments.at(1).size() > 6) {
                if (arguments.at(1).substr(0, 7) == "\001ACTION") {
                    // This is a ACTION message.
                    str_buffer = string(BOLDMAGENTA) + "\u2022 " + message.get_nickname() + " " + arguments.at(1).substr(8) + string(RESET) + "\r\n";
                }
            }
        }
    } else if (message.get_command() == "JOIN") {
        // Someone joined the channel.
        // TODO: Check if this isn't adding more than one time when another user joins the channel.
        // If it is move this to the repl loop and create an eval return for it.
        if (!arguments.empty()) {
            channels.add(arguments.at(0).substr(1, arguments.at(0).find(":") - 1));
        }

        str_buffer = string(BOLDGREEN) + "> " + string(RESET) + message.get_nickname() + " joined " + arguments.at(0) + "\r\n";
    } else if (message.get_command() == "PART") {
        // Someone left the channel.
        str_buffer = string(BOLDRED) + "< " + string(RESET) + message.get_nickname() + " left\r\n";
    } else if (message.get_command() == "KICK") {
        // Someone got kicked.
        str_buffer = string(BOLDRED) + "<< " + message.get_nickname() + " got kicked from " + arguments.at(0) + " (" + arguments.at(2) + ")" + string(RESET) + "\r\n";
    } else if (message.get_command() == "MODE") {
        // Changing modes.
        str_buffer = string(BOLDBLUE) + "* " + string(RESET) + message.get_nickname() + " set mode ";

        for (unsigned int i = 0; i < arguments.size(); i++) {
            str_buffer += arguments.at(i) + " ";
        }

        str_buffer += "\r\n";
    } else if (message.get_command() == "QUIT") {
        // Quitting.
        str_buffer = string(BOLDRED) + "<< " + string(RESET) + message.get_nickname() + " quit (" + arguments.at(0) + ")\r\n";
    } else if (message.get_command() == "ERROR") {
        // Oh noes! Error!
        str_buffer = string(BOLDRED) + "Error: " + arguments.at(0) + string(RESET) + "\r\n";
    } else if (message.get_command() == "NICK") {
        // Someone is chaning the nick.
        str_buffer = string(BOLDBLUE) + "* " + color_string(message.get_nickname(), true) + " is now known as " + color_string(arguments.at(0), true) + "\r\n";
    } else {
        // Might be a server message, so let's check for the reply code.
        int reply_code = message.get_reply_code();
        ostringstream stream;

        switch (reply_code) {
            case RPL_TOPIC:
                // Got a topic.
                str_buffer = string(BOLDWHITE) + "Topic: " + "\"" + arguments.at(2) + "\"" + string(RESET) + "\r\n";
                break;
            case RPL_TOPICWHOTIME:
                // Got who set the topic.
                str_buffer = "Topic set by " +  arguments.at(2).substr(0,  arguments.at(2).find('!')) + "\r\n";
                break;
            case RPL_NAMREPLY:
                // Got a list of the people online.
                stream << "Users: ";
                
                str_buffer = arguments.at(3);
                while (str_buffer.find(" ") != string::npos) {
                    size_t pos = str_buffer.find(" ");
                
                    stream << color_string(str_buffer.substr(0, pos), true) << " ";
                    str_buffer.erase(0, pos + 1);
                }
                
                if (!str_buffer.empty()) {
                    stream << color_string(str_buffer, true);
                }
                
                stream << "\r\n";
                str_buffer = stream.str();
                break;
            case RPL_ENDOFNAMES:
                // (Ignored) Got the end of the online names list
                str_buffer = "";
                break;
            default:
                // Got some other kind of message that we haven't covered.
                str_buffer = "";

                for (size_t i = 1; i < arguments.size(); i++) {
                    str_buffer += arguments.at(i) + " ";
                }

                str_buffer += "\r\n";
                break;
        }
    }

    if (arguments.at(0).at(0) == '#') {
        channels.cache(arguments.at(0).substr(1), str_buffer);
    }

    return str_buffer;
}

/**
 * Check if the message should be echoed.
 *
 * \return true if the message should be echoed.
 */
bool Pretty_Print_Message::echo_message() {
    return echo;
}
