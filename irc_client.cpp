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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include "irc_client.h"
#include "message.h"

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
    int bytes_sent = send(socket_descriptor, buffer, len, 0);

    if (bytes_sent == 0) {
        return false;
    } else {
        return true;
    }
}

void IRC_Client::message_handler(char *buffer) {
    Message message(buffer);

    if (message.get_command() == "PING") {
        IRC_Client::send_data("PONG " + message.get_command_args().at(0) + "\r\n");
    }
}

void *IRC_Client::handle_recv(void) {
    // recv some data.
    int numbytes;
    char buffer[MAXDATASIZE];

    while (true) {
        numbytes = recv(socket_descriptor, buffer, MAXDATASIZE - 1, 0);
        buffer[numbytes] = '\0';

        if (!repl.has_started) {
            cout << buffer;
        } else {
            string str_buffer(buffer);
            int diff = (repl.current_str.length() + 2) - str_buffer.length();
            cout << repl.current_str.length() + 2 << endl;

            if (diff > 0) {
                for (int i = 0; i < diff; i++) {
                    cout << "Found - ";
                    str_buffer.append(" ");
                }

                cout << endl;
            }

            cout << "\r" << str_buffer;
            repl.rewrite();
        }
 
        message_handler(buffer);    

        if (numbytes == 0) {
            cout << "Connection terminated" << endl;
        }
    }
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
    if ((socket_descriptor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("Error while assigning the socket descriptor");
    }

    // Connect!
    if (connect(socket_descriptor, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(socket_descriptor);
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

        send_data(repl.read().c_str());
        cout << repl.current_str << endl;
    }
}
