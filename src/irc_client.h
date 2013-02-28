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
		
		// Destructor
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
		bool read();
		
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
		
		// Current message being processed and a vector of all messages
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
		////////////////////////////////////////////////////////////////////////////
		//	Helper functions for sending/receiving messages from the IRC server
		////////////////////////////////////////////////////////////////////////////
		
		// Cleans the mess.
		void clean_mess();

		// Send the data string to the server and return the number of bytes sent
		int send_data(std::string data);
		
		// Parse and process the given string buffer
		bool message_handler(const char *buffer);
		
		void *handle_recv(void);
		static void *handle_recv_thread_helper(void *context);
		
		// History functions for saving the input from the user to recall later on
		void add_history();
		void back_history();
		void forward_history();
		
		// socket file descriptor for the IRC server
		int sd;
		
		// True if we are connected
		bool connected;
		
		std::vector<std::string> autojoin_channels;
};

#endif	// LEAF_IRC_CLIENT_H_
