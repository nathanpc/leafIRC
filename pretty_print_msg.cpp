/**
 *  pretty_print_msg.cpp
 *
 *  @author Nathan Campos
 */

#include <string>

#include "pretty_print_msg.h"
#include "message.h"
#include "channels.h"
#include "color.h"
#include "irc_reply_codes.h"
using namespace std;

Pretty_Print_Message::Pretty_Print_Message(const char *_buffer) {
    buffer = _buffer;
    echo = true;
}

string Pretty_Print_Message::generate(Message &message, Channels &channels) {
    string str_buffer(buffer);
    vector<string> arguments = message.get_command_args();
    // TODO: Add ACTION to the list of special parsing messages.

    if (message.get_command() == "PRIVMSG") {
        if (arguments.at(0).at(0) == '#') {
            // Channel message.
            if (arguments.at(0) != "#" + channels.list.at(channels.current)) {
                echo = false;
            }

            // TODO: Generate a color for each nick based on its letters.
            str_buffer = string(BOLDWHITE) + "<" + message.get_nickname() + "> " + string(RESET) + arguments.at(1) + "\r\n";
        }
    } else if (message.get_command() == "JOIN") {
        // TODO: Check if this isn't adding more than one time when another user joins the channel.
        // If it is move this to the repl loop and create an eval return for it.
        if (!arguments.empty()) {
            channels.add(arguments.at(0).substr(1, arguments.at(0).find(":") - 1));
        }

        str_buffer = string(BOLDGREEN) + "> " + string(RESET) + message.get_nickname() + " joined " + arguments.at(0) + "\r\n";
    } else if (message.get_command() == "PART") {
        str_buffer = string(BOLDRED) + "< " + string(RESET) + message.get_nickname() + " left\r\n";
    } else if (message.get_command() == "KICK") {
        str_buffer = string(BOLDRED) + "<< " + message.get_nickname() + " got kicked from " + arguments.at(0) + " (" + arguments.at(2) + ")" + string(RESET) + "\r\n";
    } else if (message.get_command() == "MODE") {
        str_buffer = string(BOLDBLUE) + "* " + string(RESET) + message.get_nickname() + " set mode ";
        for (unsigned int i = 0; i < arguments.size(); i++) {
            str_buffer += arguments.at(i) + " ";
        }
        str_buffer += "\r\n";
    } else if (message.get_command() == "QUIT") {
        str_buffer = string(BOLDRED) + "<< " + string(RESET) + message.get_nickname() + " quit (" + arguments.at(0) + ")\r\n";
    } else if (message.get_command() == "ERROR") {
        str_buffer = string(BOLDRED) + "Error: " + arguments.at(0) + string(RESET) + "\r\n";
    } else {
        // Might be a server message, so let's check for the reply code.
        int reply_code = message.get_reply_code();

        switch (reply_code) {
            case RPL_TOPICWHOTIME:
                str_buffer = "Topic set by " +  arguments.at(2).substr(0,  arguments.at(2).find('!')) + "\r\n";
                break;
            case RPL_TOPIC:
                str_buffer = string(BOLDWHITE) + "Topic: " + "\"" + arguments.at(2) + "\"" + string(RESET) + "\r\n";
                break;
            case RPL_NAMREPLY:
                //[01:48:50] :irc.arcti.ca 353 leafirc = #leafirc :leafirc nathanpc @vivid_
                //[01:48:50] :irc.arcti.ca 366 leafirc #leafirc :End of /NAMES list.
                break;
            case RPL_ENDOFNAMES:
                // Ignored
                str_buffer = "";
                break;
            default:
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
