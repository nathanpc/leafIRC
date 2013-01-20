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
#include "pretty_print_msg.h"
#include "color.h"

#define MAXDATASIZE 256
using namespace std;

IRC_Client::IRC_Client(string _server, string _port, string _server_password) {
    server = _server;
    port = _port;
    server_password = _server_password;
    connected = false;
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
		exit(1);
	}
	
    message = Message(buffer);

    if (message.get_command() == "PING") {
    	// There is an assumption here that
    	// message.get_command_args() is not empty
        IRC_Client::send_data("PONG " + message.get_command_args().at(0) +
        	"\r\n");
    } else {
        // Messages that need to be echoed.
        Pretty_Print_Message pretty_print(buffer);
        string str_buffer = pretty_print.generate(message, channels);
		
        if (pretty_print.echo_message()) {
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
                    if (str_buffer != "") {
                        str_buffer = time_str + str_buffer;
                        cout << str_buffer;
                    }
                #endif
            } else {
				repl.clear();
                #ifdef DEBUG
				    cout << message << "\n";
                #else
                    if (str_buffer != "") {
                        str_buffer = time_str + str_buffer;
                        cout << "\r" << str_buffer;
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
    
    // Check if there was an error
    if (numbytes == -1) {
    	perror("IRC_Client::handle_recv");
    	exit(EXIT_FAILURE);
    }

    connected = false;
    
    return NULL;
}

void * IRC_Client::handle_recv_thread_helper(void *context) {
	return ((IRC_Client *)context)->handle_recv();
}

void IRC_Client::start_connection() {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(server.c_str(), port.c_str(), &hints, &servinfo);
    
    if (res != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
    }

    // Setup the socket descriptor.
    sd = socket(servinfo->ai_family, servinfo->ai_socktype,
    	servinfo->ai_protocol);
    
    if (sd == -1) {
        perror("socket");
    }

    // Connect!
    if (connect(sd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sd);
        perror("connect");
    }
    
    connected = true;
    
    // Free the server information, we don't need it anymore.
    freeaddrinfo(servinfo);
    pthread_create(&thread, NULL, &handle_recv_thread_helper, this);
}

int IRC_Client::run()
{
	// Check if we're not connected
	if(!is_connected())
	{
		cerr << "Error: \"no connection found\"\n";
		return EXIT_FAILURE;
	}
	
	// Send the first authentication messages to the server.
	send_data("NICK " + nick + "\r\n");
	send_data("USER " + username + " 0 * :" + realname + "\r\n");
	
    while (is_connected()) {
        // Read the user input.
		repl.read();
        
        // Check if the user input is ready to be processed.
        if (repl.string_is_ready) {
            if (!repl.external_command.empty()) {
                // Command.
                string command = repl.external_command.at(0);

                if (command == "switch") {
                    // Switching channels.
                    channels.current =
                    	channels.find_index(repl.external_command.at(1));

                    repl.clear();
                    cout << "\r" << BOLDBLUE << "Switching to #"
                    	 << repl.external_command.at(1) << RESET << "\r\n"
                    	 << channels.load_cache(repl.external_command.at(1));
                    
                    repl.current_str = "";
                } else if (command == "message") {
                    // Just a normal message that needs
                    // to be sent to the current channel.
                    if (channels.current != -1) {
                        // Include the command to the message.
                        string send_msg = "PRIVMSG #" +
                        	channels.list.at(channels.current) + " :" +
                        	repl.current_str;
                        
                        // TODO: Get nick.
                        string cache_msg = string(BOLDWHITE) + "<Me> " +
                        	string(RESET) + repl.current_str;
                        channels.cache(channels.list.at(channels.current),
                        	cache_msg);
                        send_data(send_msg.c_str());
                    }
                } else if (command == "msg") {
                    // A better PRIVMSG.
                    string send_msg = "PRIVMSG " +
                    	repl.external_command.at(1) + " :" +
                    	repl.external_command.at(2) + "\r\n";
                    send_data(send_msg.c_str());
                } else if (command == "me") {
                    // ACTIONs!
                    string curr_channel = channels.list.at(channels.current);
                    string send_msg = "PRIVMSG #" + curr_channel +
                    	" :\001ACTION " + repl.external_command.at(1) + "\r\n";

                    send_data(send_msg.c_str());
                }
            } else {
                // Just send raw stuff.
                send_data(repl.current_str.c_str());
            }
        }
    }
    
    return EXIT_SUCCESS;
}

// Return true if the client is connected to a server
bool IRC_Client::is_connected() const
{
	return connected;
}
