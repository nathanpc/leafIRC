/**
 *  irc_client.cpp
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <ctime>

#include "irc_client.h"
#include "message.h"
#include "channels.h"
#include "color.h"
#include "irc_reply_codes.h"

#define MAXDATASIZE 256
using namespace std;

IRC_Client::IRC_Client(string _server, string _port, string _server_password) {
    server = _server;
    port = _port;
    server_password = _server_password;
}

void IRC_Client::setup_user(string _nick, string _username, string _realname) {
    nick = _nick;
    username = _username;
    realname = _realname;
}

bool IRC_Client::send_data(string data) {
    const char *buffer = data.c_str();
    int len = strlen(buffer);
    int bytes_sent;
    
    if ((bytes_sent = send(sd, buffer, len, 0)) == -1) {
    	perror("IRC_Client::send_data(): send");
    	exit(EXIT_FAILURE);
    }

    return bytes_sent;
}

void IRC_Client::message_handler(const char *buffer) {
	// Check if the buffer is empty
	if (strlen(buffer) == 0) {
		cerr << "Error: \"buffer is empty\"\n";
		exit(0);
	}
	
    Message message(buffer);
    string str_buffer(buffer);

    if (message.get_command() == "PING") {
    	// There is an assumption here that message.get_command_args() is not empty
        IRC_Client::send_data("PONG " + message.get_command_args().at(0) + "\r\n");
    } else {
        // Messages that need to be echoed.
        vector<string> arguments = message.get_command_args();
        bool echo = true;

        if (message.get_command() == "PRIVMSG") {
            if (arguments.at(0).at(0) == '#') {
                // Channel message.
                if (arguments.at(0) != "#" + channels.list.at(channels.current)) {
                    echo = false;
                }

                // TODO: Generate a color for each nick based on its letters.
                str_buffer = string(BOLDWHITE) + "<" + message.get_nickname() + "> " + string(RESET) + arguments.at(1) + "\r\n";
                channels.cache(arguments.at(0).substr(1), str_buffer);
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
		
        if (echo) {
            time_t rawtime;
            struct tm *timeinfo;
            char time_str[12];

            // Create a timestamp for the message.
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(time_str, 12, "[%H:%M:%S] ", timeinfo);

            if (!repl.has_started) {
                #ifdef DEBUG
				    cout << "\n" << message << "\n";
                #else
                    cout << time_str << str_buffer;
                #endif
            } else {
				repl.clear();
                #ifdef DEBUG
				    cout << message << "\n";
                #else
                    if (str_buffer != "") {
                        cout << "\r" << time_str << str_buffer;
                    }
                #endif
				repl.rewrite();
            }
        }
    }
}

void *IRC_Client::handle_recv(void) {
    // recv some data.
    int numbytes;
    char buffer[MAXDATASIZE];
    static string sbuf;
    size_t pos;

    while ((numbytes = recv(sd, buffer, MAXDATASIZE - 1, 0)) > 0) {
       	// NULL-terminate the buffer
       	buffer[numbytes] = '\0';
       	
       	// Append the buffer instead of assigning so we don't lose anything
       	sbuf += buffer;
        
        // Search for the CR (\r) character followed by the LF (\n) character,
        while ((pos = sbuf.find("\r\n")) != string::npos) {
        	// Copy the whole message, including the CRLF at the end
        	string msg = sbuf.substr(0, pos + 2);
        	
        	// Erase the msg from sbuf
        	sbuf.erase(0, msg.size());
        	
		    // Handle the received message
		    message_handler(msg.c_str());
		}
    }
    
    // Connection closed or there was an error
    if (numbytes == -1) {
    	perror("IRC_Client::handle_recv");
    	exit(EXIT_FAILURE);
    }

    repl.~REPL();
    exit(0);
    return NULL;
}

void IRC_Client::start_connection() {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int res;
    if ((res = getaddrinfo(server.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
    }

    // Setup the socket descriptor.
    if ((sd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("Error while assigning the socket descriptor");
    }

    // Connect!
    if (connect(sd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sd);
        perror("Couldn't connect");
    }
    
    freeaddrinfo(servinfo);  // Free the server information, we don't need it anymore.
    pthread_create(&thread, NULL, &handle_recv_thread_helper, this);

    bool just_connected = true;
    while (true) {
        if (just_connected) {
            send_data("NICK " + nick + "\r\n");
            send_data("USER " + username + " 0 * :" + realname + "\r\n");  // TODO: Set user mode.

            just_connected = false;
        }

        repl.read();

        if (repl.string_is_ready) {
            if (!repl.external_command.empty()) {
                // Command.
                string command = repl.external_command.at(0);

                if (command == "switch") {
                    channels.current = channels.find_index(repl.external_command.at(1));

                    repl.clear();
                    cout << "\r" << BOLDBLUE << "Switching to #" << repl.external_command.at(1) << RESET << "\r\n";
                    cout << channels.load_cache(repl.external_command.at(1));
                    repl.current_str = "";
                } else if (command == "message") {
                    if (channels.current != -1) {
                        string send_msg = "PRIVMSG #" + channels.list.at(channels.current) + " :" + repl.current_str;

                        // TODO: Get nick.
                        string cache_msg = string(BOLDWHITE) + "<Me> " + string(RESET) + repl.current_str;
                        channels.cache(channels.list.at(channels.current), cache_msg);
                        send_data(send_msg.c_str());
                    }
                }
            } else {
                // Just send raw stuff.
                send_data(repl.current_str.c_str());
            }
        }
    }
}
