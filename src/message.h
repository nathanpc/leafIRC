/**
 *  message.h
 *
 *  @author Nathan Campos
 */

#ifndef LEAF_MESSAGE_H_
#define LEAF_MESSAGE_H_

#include <iosfwd>
#include <string>
#include <vector>

class Message {
public:
	// Constructors.
	Message();
	Message(const char* s);
	Message(const Message& m);
	Message& operator=(const Message& m);
	
	// Output operator for output streams
	friend std::ostream& operator<<(std::ostream& out, const Message& m);
	
	std::string get_server();
	std::string get_nickname();
	std::string get_username();
	std::string get_hostname();
	std::string get_command();
	std::vector<std::string> get_command_args();
	int get_reply_code();

private:
	std::string _raw;
	std::string raw;
	std::string server;
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string cmd;
	std::vector<std::string> args;
	
	bool parse_server();
	std::string parse_nickname();
	std::string parse_username();
	std::string parse_hostname();
	std::string parse_command();
	std::string parse_arguments();
	std::string strip_end_newline(std::string line);
	
	bool parse();
	
	void copy(const Message& rhs);
};

#endif	// LEAF_MESSAGE_H_
