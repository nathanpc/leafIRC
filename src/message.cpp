/**
 *  message.cpp
 *
 *  @author Nathan Campos
 */
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "message.h"
using namespace std;

/**
 * Empty Message constructor.
 */
Message::Message() {}

/**
 * Message constructor.
 *
 * \param s A message string.
 */
Message::Message(const char* s) {
	if ((raw = _raw = strip_end_newline(s)).empty()) {
		cerr << "Message::Message() \"message is empty\"\n";
		exit(0);
	}
	
	parse();
}

/**
 * Message constructed from another Message.
 *
 * \param m Pointer to a Message
 */
Message::Message(const Message& m) {
	copy(m);
}

/**
 * Set the Message
 *
 * \param m Pointer to a Message.
 * \return This.
 */
Message& Message::operator=(const Message& m) {
	copy(m);
	return *this;
}

/**
 * Copy the Message.
 *
 * \param m Pointer to a message
 */
void Message::copy(const Message& m) {
	// Check for self-assignment
	if (this != &m) {
		_raw		= m._raw;
		raw			= m.raw;
		server		= m.server;
		nickname	= m.nickname;
		username	= m.username;
		hostname	= m.hostname;
		cmd			= m.cmd;
		
		args.clear();
		std::copy(m.args.begin(), m.args.end(), back_inserter(args));
	}
}

/**
 * Parse the raw string and check if it's actually multiple messages.
 *
 * \param s Raw message string.
 * \return A message.
 */
static string changeNewlines(const string& s) {
	if (s.empty()) {
		return s;
	}
	
	string retval = s;
	size_t i, sz;
	
	// Loop through the given string, changing any \r or \n
	for (i = 0, sz = retval.size(); i < sz - 1; ++i) {
		if (retval[i] == '\r' && i + 1 < sz && retval[i + 1] == '\n') {
			retval[i] = '\\';
			retval[i + 1] = 'n';
		}
	}
	
	return retval;
}

/**
 * Overrides the << operator.
 *
 * \param out Pointer to a ostream.
 * \param m Pointer to a Message.
 * \return The final ostream.
 */
ostream& operator<<(ostream& out, const Message& m) {
    #ifdef DEBUG
	    out << "\nraw = " << changeNewlines(m._raw) << "\n";
	    out << "server = \"" << changeNewlines(m.server) << "\"\tnickname = \""
		    << changeNewlines(m.nickname) << "\"\tusername = \""
		    << changeNewlines(m.username) << "\"\thostname = \""
    		<< changeNewlines(m.hostname) << "\"\tcommand = \""
	    	<< changeNewlines(m.cmd) << "\"\narguments: ";
    #endif
		
	if (m.args.empty()) {
		out << "none\n";
	} else {
		size_t i, sz;
		
		for (i = 0, sz = m.args.size(); i < sz - 1; ++i) {
			out << "\"" << changeNewlines(m.args[i]) << "\", ";
		}
		
		// 'i' should be less than 'sz' but check anyways
		if (i < sz) {
			out << "\"" << changeNewlines(m.args[i]) << "\"\n";
		}
	}
	
	return out;
}

/**
 * Parse the message and populate the internal variables.
 *
 * \return Always true.
 */
bool Message::parse() {
	// Check when parsing for the server, if the message begins with NOTICE
	if (parse_server()) {
        parse_nickname();
		parse_username();
		parse_hostname();
	}

	parse_command();
	parse_arguments();
	
	return true;
}

/**
 * Parse the server address from the raw message.
 *
 * \return true if we could parse a server address, false if we couldn't.
 */
bool Message::parse_server() {
	// Validate the raw string before we start parsing.
	// It must not be empty and the first char should be a colon ':'
	if (raw.empty() || raw[0] != ':') {
		return false;
	} else if(raw[0] == ':') {
		raw.erase(0, 1);
	}
	
	size_t pos;
	bool ans = false;
	
	// Look for the first of either an exclamation mark or a space
	for (pos = 0; pos < raw.size(); ++pos) {
		if (raw[pos] == '!') {
			ans = true;
			break;
		} else if(raw[pos] == ' ') {
			ans = false;
			break;
		}
	}
	
	// Check if neither a ! or a space were found
	if (pos >= raw.size()) {
		cerr << "Error: \"invalid IRC message\"\n";
		exit(EXIT_FAILURE);
	}
	
	server = raw.substr(0, pos);
    if (server.find('.') != string::npos) {
    	raw.erase(0, pos + 1);
    } else {
        server = "";
    }

	return ans;
}

/**
 * Parse my own nickname from the raw message.
 *
 * \return Nickname.
 */
string Message::parse_nickname() {
	size_t pos;
	
	// Look for the at symbol, if not found return an empty string
	if ((pos = raw.find('!')) == string::npos) {
		return string();
	}
	
	// Save the nickname and remove it and the at symbol from the raw string
	nickname = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	return nickname;
}

/**
 * Parse my own username from the raw message.
 *
 * \return Username.
 */
string Message::parse_username() {
	size_t pos;
	
	// Look for the at symbol, if not found return an empty string
	if ((pos = raw.find('@')) == string::npos) {
		return string();
	}
	
	// Save the username and remove it and the at symbol from the raw string
	username = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	return username;
}

/**
 * Parse my own hostname from the raw message.
 *
 * \return Hostname.
 */
string Message::parse_hostname() {
	size_t pos;
	
	// Get the index for the first space character and check if it is not found
	if ((pos = raw.find(' ')) == string::npos) {
		return string();
	}
	
	// Save the hostname and remove it and the space from the raw string
	hostname = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	return hostname;
}

/**
 * Parse the command from the raw message.
 *
 * \return Command.
 */
string Message::parse_command() {
	size_t pos = raw.find(' ');
	
	cmd = raw.substr(0, pos);
	
	// Either erase the rest of the string or the command and the space
	raw.erase(0, pos == string::npos ? pos : pos + 1);
	
	return cmd;
}

/**
 * Parse arguments from the raw message.
 *
 * \return String of arguments.
 */
string Message::parse_arguments() {
	// Check if we've already parsed the arguments
    if (!raw.empty()) {
		// Parse the argument(s) from the msg string
		while (!raw.empty()) {
		    size_t current_pos;
		    
		    // First check if this is the last "colon" argument
		    if (raw[0] == ':') {
		    	// Push the last argument onto vector and erase the raw string
		    	args.push_back(raw.substr(1));
		    	raw.clear();
				return args.back();
		    }
		    
		    // We don't need to check if it was found or not
		    current_pos = raw.find(' ');
		    
		    // Add the last parsed argument to the arguments vector
		    args.push_back(raw.substr(0, current_pos));
		    
		    // Erase the last parsed argument from the raw string,
		    // possibly making it empty
		    raw.erase(0, current_pos == string::npos ?
		    	current_pos : current_pos + 1);
		}
	}
	
	return args.back();
}

/**
 * Remove the final newlines from the message.
 *
 * \return Message without the final newlines.
 */
string Message::strip_end_newline(string line) {
    return line.erase(line.find("\r\n"), 2);
}

/**
 * Server getter.
 *
 * \return Server.
 */
string Message::get_server() {
	return server;
}

/**
 * Nickname getter.
 *
 * \return Nickname.
 */
string Message::get_nickname() {
	return nickname;
}

/**
 * Username getter.
 *
 * \return Username.
 */
string Message::get_username() {
	return username;
}

/**
 * Hostname getter.
 *
 * \return Hostname.
 */
string Message::get_hostname() {
	return hostname;
}

/**
 * Command getter.
 *
 * \return Command.
 */
string Message::get_command() {
    return cmd;
}

/**
 * Command arguments getter.
 *
 * \return Command arguments.
 */
vector<string> Message::get_command_args() {
	return args;
}

/**
 * Reply code getter.
 *
 * \return Reply code.
 */
int Message::get_reply_code() {
    return atoi(cmd.c_str());
}
