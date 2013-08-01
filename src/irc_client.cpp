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

#include "conio/conio.h"
#include "irc_client.h"
#include "message.h"
#include "channels.h"
#include "pretty_print_msg.h"
#include "color.h"
#include "notification.h"

#define MAXDATASIZE 256
using namespace std;

////////////////////////////////////////////////////////////////////////////////
//				Public member functions for the IRC_Client class
////////////////////////////////////////////////////////////////////////////////

/**
 *  Constructs IRC_Client.
 *
 *  \param _server IRC server address.
 *  \param _port IRC server port.
 *  \param _server_password IRC server password.
 */
IRC_Client::IRC_Client(string _server, string _port, string _server_password) {
	server = _server;
	port = _port;
	server_password = _server_password;
	connected = false;

	has_started = false;
	string_is_ready = false;
	input_marker = string(BOLDCYAN) + ":: " + string(RESET);
	history_current_position = 0;has_started = false;
}


/**
 *  Destructs IRC_Client.
 */
IRC_Client::~IRC_Client() {
	clean_mess();
}

void IRC_Client::clean_mess() {
	clear();
	Conio::initTermios(1);
	Conio::resetTermios();
	system("stty echo cooked");
}

/**
 * Populate the user-related variables.
 *
 * \param _nick User's nickname.
 * \param _username User's username.
 * \param _realname User's real name.
 * \param _nickserv NickServ password for authentication.
 * \param _autojoin_channels Channels to join on connect.
 */
void IRC_Client::setup_user(string _nick, string _username, string _realname,
						string _nickserv, vector<string> _autojoin_channels) {
	nick = _nick;
	username = _username;
	realname = _realname;
	nickserv = _nickserv;
	autojoin_channels = _autojoin_channels;
}

/**
 * Connect to the IRC server using the string in the server member variable.
 */
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
	
	// We have connected to the server
	connected = true;
	
	// Free the server information, we don't need it anymore.
	freeaddrinfo(servinfo);
	
	// Create the thread that will handle messages from the server
	pthread_create(&thread, NULL, &handle_recv_thread_helper, this);

	// Send the first authentication messages to the server.
	#ifdef DEBUG
	cout << endl << "NICK USER: " << nick << " " << username << endl;
	#endif

	send_data("NICK " + nick + "\r\n");
	send_data("USER " + username + " 0 * :" + realname + "\r\n");
}

/**
 * Closes the connection if it's connected.
 */
void IRC_Client::close_connection() {
	// First check if we are already connected
	if (is_connected()) {
		// If so, disconnect and check for any errors
		if (close(sd) != 0) {
			perror("Error while trying to close the connection");
		}
		
		// We disconnected correctly
		connected = false;
	}
}

/**
 * Run the main loop for the client.
 *
 * \return 0 on success, -1 on failure.
 */
int IRC_Client::run() {
	// Check if we're not connected
	if (!is_connected()) {
		cerr << "Error: \"No connection found\"\n";
		return EXIT_FAILURE;
	}
	
	// Loop until we can't get anymore input
	while (is_connected() && read()) {
		// Check if the user input is ready to be processed.
		if (string_is_ready) {
			if (!external_command.empty()) {
				// Command.
				string command = external_command.at(0);

				if (command == "switch") {
					// Switching channels.
					channels.current =
					channels.find_index(external_command.at(1));

					clear();
					cout << "\r" << BOLDBLUE << "Switching to "
						<< external_command.at(1) << RESET << "\r\n"
						<< channels.load_cache(external_command.at(1));

					current_str = "";
				} else if (command == "message") {
					// Just a normal message that needs
					// to be sent to the current channel.
					if (channels.current != -1) {
						// Include the command to the message.
						string send_msg = "PRIVMSG " +
							channels.list.at(channels.current) + " :" +
							current_str;

						string cache_msg = string(BOLDWHITE) + "<" + nick + "> " +
						string(RESET) + current_str;
						channels.cache(channels.list.at(channels.current),
						cache_msg);
						send_data(send_msg.c_str());
					}
				} else if (command == "msg") {
					// A better PRIVMSG.
					string send_msg = "PRIVMSG " +
					external_command.at(1) + " :" +
					external_command.at(2) + "\r\n";
					send_data(send_msg.c_str());
				} else if (command == "me") {
					// ACTIONs!
					string curr_channel = channels.list.at(channels.current);
					string send_msg = "PRIVMSG " + curr_channel +
					" :\001ACTION " + external_command.at(1) + "\r\n";

					send_data(send_msg.c_str());
				} else if (command == "chats") {
					// List the active chats.
					clear();
					cout << "\rActive chats: " << BOLDWHITE;

					for (size_t i = 0; i < channels.list.size(); i++) {
						cout << channels.list.at(i) << " ";
					}

					cout << RESET << endl;
					current_str = "";
				}
			} else {
				// Just send raw stuff.
				send_data(current_str.c_str());
			}
		}
	}
	
	return EXIT_SUCCESS;
}

/**
 * Checks if is connected to a server.
 *
 * \return true if the client is connected to a server
 */
bool IRC_Client::is_connected() const {
	return connected;
}

/**
 * Clear the prompt from the console.
 */
void IRC_Client::clear() {
	int curr_input_length = current_str.length() + input_marker.length();

	for (int i = 0; i < curr_input_length; i++) {
		printf("\b \b");
	}
}

/**
 * Write a prompt to the console.
 */
void IRC_Client::rewrite() {
	if (channels.list.size() != 0) {
		// Print the marker with the current channel.
		input_marker = string(BOLDCYAN) + channels.list.at(channels.current)
			+ ' ' + string(":: ") + RESET;
	}

	cout << input_marker + current_str << flush;
}

/**
 * Read input from stdin.
 *
 * \return true if the input is ok, false if input is something like a Ctrl+C.
 */
bool IRC_Client::read() {
	// Check if we're connected to the server or not
	if (!is_connected()) {
		cerr << "Error: \"IRC_Client::read() not connected\"\n";
		return false;
	}
	
	has_started = true;
	char curr_char;

	if (string_is_ready) {
		string_is_ready = false;
		current_str = "";
		external_command.clear();

		rewrite();
	}
	
	//printf("\n\n%d\n\n", curr_char = Conio::getche());
	curr_char = Conio::getch();
	
	// TODO: Implement nick auto-completion when the user hits tab.
	if (curr_char == 3) {
		// Control + C
		return false;
	}  else if (curr_char == 9) {
		// Tab
		string nick_part;
		size_t spos = current_str.find_last_of(" ");

		// Get the unfinished nickname after the last space.
		if (spos != string::npos) {
			nick_part = current_str.substr(spos + 1);
		} else {
			nick_part = current_str;
		}

		// Complete.
		string completion = channels.strip_user_level(channels.find_user(channels.current, nick_part, true));
		if (!completion.empty()) {
			current_str = current_str.substr(0, spos + 1).append(completion);
			clear();
			rewrite();
		}
	} else if (curr_char == 127) {
		// Backspace
		if (current_str != "") {
			current_str = current_str.substr(0, current_str.length() - 1);
			printf("\b \b");
		}
	} else if (curr_char == 27) {
		if (Conio::getche() == 91) {
			// Arrow keys
			curr_char = Conio::getche();
			if (curr_char == 65) {
				// Up
				if (history_current_position < history.size()) {
					clear();
					
                    current_str = history.at(history.size() - 1 -
						history_current_position);
					history_current_position++;
					
					rewrite();
				}
			} else if (curr_char == 66) {
				// Down
				if (history_current_position != 0) {
					clear();
					
					if (history_current_position - 1 > 0) {
						current_str = history.at(history.size() + 1 -
						history_current_position);
						history_current_position--;
					} else {
						history_current_position = 0;
						current_str = "";
					}
					
					rewrite();
				}
			} else if (curr_char == 67) {
				// Right
			} else if (curr_char == 68) {
				// Left
			}
		}
	} else if (curr_char != 10) {
		// Others
		char char_str[2];
		char_str[0] = curr_char;
		char_str[1] = '\0';
		
		current_str.append(char_str);
		printf("%c", curr_char);
	} else {
		// Return
		if (current_str != "") {
			add_history();
			history_current_position = 0;
			
			if (eval()) {
				current_str.append("\r\n");
			}
			
			printf("\n");
			string_is_ready = true;
		}
	}
	
	return true;
}

/**
 * Evaluate the input to see what type of message it is.
 *
 * \return true if the input ONLY needs \r\n to be appended to the end (no more parsing).
 */
bool IRC_Client::eval() {
	if (current_str != "" && current_str.at(0) == '/') {
		// Gets the string between "/" and the first space.
		string command = current_str.substr(1, current_str.find(" ") - 1);
	
		if (command == "switch") {
			// Switch the current channel.
			string switch_channel = current_str.substr(current_str.find(" ") + 1);
			
			external_command.push_back("switch");
			external_command.push_back(switch_channel);
		} else if (command == "msg") {
			// A better PRIVMSG.
			external_command.push_back("msg");
			
			// Get the message receiver.
			string tmp_str = current_str.substr(current_str.find(" ") + 1);
			external_command.push_back(tmp_str.substr(0, tmp_str.find(" ")));
			
			// Get the message.
			tmp_str = tmp_str.substr(tmp_str.find(" ") + 1);
			external_command.push_back(tmp_str);
		} else if (command == "me") {
			// ACTIONs!
			external_command.push_back("me");
			
			// Get the message.
			string action_msg = current_str.substr(current_str.find(" ") + 1);
			external_command.push_back(action_msg);
		} else if (command == "chats") {
			external_command.push_back("chats");
		} else {
			// Common IRC command.
			current_str = current_str.substr(1);
			return true;
		}
		
		return false;
	} else {
		// Message
		external_command.push_back("message");
		
		return true;
	}
}

////////////////////////////////////////////////////////////////////////////////
//		Helper functions sending/receiving messages from the IRC server
////////////////////////////////////////////////////////////////////////////////

/**
 * Send the data string to the server.
 *
 * \return Number of bytes sent.
 */
int IRC_Client::send_data(string data) {
	#ifdef DEBUG
	cout << "\nSend Data: \"" << data << "\"";
	#endif

	const char *buffer = data.c_str();
	int len = strlen(buffer);
	int bytes_sent = send(sd, buffer, len, 0);

	if (bytes_sent == -1) {
		perror("IRC_Client::send_data(): send");
		exit(EXIT_FAILURE);
	}

	return bytes_sent;
}

/**
 * Parse and process the given string buffer.
 *
 * \return true if no errors were thrown.
 */
bool IRC_Client::message_handler(const char *buffer) {
	// Check if the buffer is empty
	if (strlen(buffer) == 0) {
		cerr << "Error: \"buffer is empty\"\n";
		return false;
	}
	
	message = Message(buffer);

	// Add message to log so we can test the connection
	log.push_back(message);

	if (message.get_command() == "PING") {
		// There is an assumption here that
		// message.get_command_args() is not empty
		IRC_Client::send_data("PONG " + message.get_command_args().at(0) + "\r\n");
	} else if (message.get_command() == "ERROR") {
		// Clear the prompt from the console
		clear();
		
		// Get the argument(s) to the error message
		vector<string> args = message.get_command_args();
		
		// Check if there are no arguments
		if (args.empty()) {
			cerr << "Error: \"no arguments with error\"\n";
			return false;
		} else if (args.size() == 1) {
			// Make sure there is only one argument
			// Check if the string begins with "Closing Link:"
			if (args.back().substr(0, 13) == "Closing Link:") {
				// Also check if "Quit:" is in the string
				if (args.back().find("Quit:") != string::npos) {
					cerr << "\nGoodbye!\n";
					return true;
				}
				
				cerr << "Error: \"'Quit:' not found in error message\"\n";
				return false;
			}
			
			// Handle error message with one argument
			cerr << "Error: \"" << args.back() << "\"\n";
			if (args.back() == "Closing connection") {
				// Connection closed.
				clean_mess();
				exit(0);
			}

			return false;
		}
		
		// Handle error with multiple arguments
		cerr << "Error: \"unknown/unhandled error message\"\n";
		return false;
	} else {
		// Messages that need to be echoed.
		Pretty_Print_Message pretty_print(buffer, nick);
		string str = pretty_print.generate(*this, message, channels);

		if (message.get_command() == "001") {
			// Just connected to the server.
			if (nickserv != "") {
				// Auto-identify with NickServ.
				send_data("PRIVMSG NickServ :identify " + nickserv + "\r\n");
			}

			if (autojoin_channels.size() > 0) {
				for (size_t i = 0; i < autojoin_channels.size(); i++) {
					send_data("JOIN " + autojoin_channels.at(i) + "\r\n");
				}
			}
		}

		if (pretty_print.echo_message()) {
			time_t rawtime;
			struct tm *timeinfo;
			char time_str[12];

			// Create a timestamp for the message.
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			strftime(time_str, 12, "[%H:%M:%S] ", timeinfo);

			if (!has_started) {
				#ifdef DEBUG
					cout << "\n" << message << "\n";
				#else
					if (str != "") {
						str = time_str + str;
						cout << str;
					}
				#endif
			} else {
				clear();
				#ifdef DEBUG
					cout << message << "\n";
				#else
					if (str != "") {
						str = time_str + str;
						cout << "\r" << str;
					}
				#endif
				rewrite();
			}
		}
	}
	
	return true;
}

/**
 * Handles the information that was received from the server.
 */
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
			if (!message_handler(msg.c_str())) {
				connected = false;
				return NULL;
			}
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

/**
 * Just a little hack to make pthread work with a C++ class.
 */
void *IRC_Client::handle_recv_thread_helper(void *context) {
	return ((IRC_Client *)context)->handle_recv();
}

////////////////////////////////////////////////////////////////////////////////
//	History functions for saving the input from the user to recall later on
////////////////////////////////////////////////////////////////////////////////

/**
 * Add the current input to the input history.
 */
void IRC_Client::add_history() {
	if (history.size() >= 100) {
		history.erase(history.begin());
	}

	history.push_back(current_str);
}

/**
 * Goes back in the history list and populates the input field.
 */
void IRC_Client::back_history() {
	if (history_current_position < history.size()) {
		clear();

		current_str = history.at(history.size() - 1 - history_current_position);
		history_current_position++;

		rewrite();
	}
}

/**
 * Goes forward in the history list and populates the input field.
 */
void IRC_Client::forward_history() {
	if (history_current_position != 0) {
		clear();

		if (history_current_position - 1 > 0) {
			current_str = history.at(history.size() + 1 - history_current_position);
			history_current_position--;
		} else {
			history_current_position = 0;
			current_str = "";
		}

		rewrite();
	}
}
