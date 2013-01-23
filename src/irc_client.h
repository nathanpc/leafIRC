/**
 *  irc_client.h
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#ifndef LEAF_IRC_CLIENT_H_
#define LEAF_IRC_CLIENT_H_

#include <string>
#include <vector>

#include "channels.h"
#include "message.h"

class IRC_Client {
public:
	// Constructor.
	IRC_Client(std::string _server, std::string _port = "6667",
		std::string _server_pass = "");
	
	~IRC_Client();
	
	// Using the info given, saved the users details for when we connect
	void setup_user(std::string _nick, std::string _username,
		std::string _realname, std::string _nickserv,
		std::vector<std::string> _autojoin_channels);
	
	// Connect to the IRC server using the string in the server member variable
	void start_connection();
	
	// If connected, close the connection to the server
	void close_connection();
	
	// Run the main loop for the client and return 0 on success, -1 on failure
	int run();
	
	// Return true if the client is connected to a server
	bool is_connected() const;
	
	// Clear the prompt from the console
	void clear();
	
	// Write a prompt to the console
	void rewrite();
	
	// Read input from stdin
	void read();
	
	// Evaluate the input to see what type of message it is
	bool eval();
	
	pthread_t thread;
	Channels channels;
	
	// Server configuration.
	std::string server;
	std::string port;
	std::string server_password;
	
	// Client configuration.
	std::string nick;
	std::string username;
	std::string realname;
	std::string nickserv;
	
	// Current message being processed
	Message message;
	
	std::vector<Message> log;
	
	bool has_started;
	bool string_is_ready;
	std::string input_marker;
	std::vector<std::string> external_command;
	
	std::string current_str;
	unsigned int history_current_position;
	std::vector<std::string> history;

private:
	// Helper functions sending/receiving messages from the IRC server
	bool send_data(std::string data);
	void message_handler(const char *buffer);
	void *handle_recv(void);
	static void *handle_recv_thread_helper(void *context);
	
	// REPL functions for saving the input from the user to recall later on
	void add_history();
	void back_history();
	void forward_history();
	
	int sd;  // socket_descriptor
	
	// True if we are connected
	bool connected;
	std::vector<std::string> autojoin_channels;
};

#endif	// LEAF_IRC_CLIENT_H_
