/**
 *  message.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "message.h"
using namespace std;

Message::Message(const string& s)
{
	raw = strip_end_newline(s);
	parse_server();
	parse_hostname();
	parse_command();
	parse_arguments();
}

string Message::parse_server()
{
	// Validate the raw string before we start parsing.
	// It must not be empty and the first char should be a colon ':'
	if(raw.empty() || raw[0] != ':')
	{
		return string();
	}
	
	size_t exclPos;
	
	// Get the index for the exclamation mark and check if it's not found
	if((exclPos = raw.find('!')) == string::npos)
	{
		return string();
	}
	
	// Save the server/nick name from the raw string
	server = raw.substr(0, exclPos);
	
	// Remove the server/nick name plus exclamation mark from the raw string
	raw.erase(0, exclPos + 1);
	
	return server;
}

string Message::parse_hostname()
{
	size_t pos;
	
	// Get the index for the at symbol '@' and check if it is not found
	if((pos = raw.find('@')) == string::npos)
	{
		return string();
	}
	
	// Parse the username and remove it and the '@' from the raw string
	username = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	// Get the index of the next space character and check if it's not found
	if((pos = raw.find(' ')) == string::npos)
	{
		return string();
	}
	
	// Parse the hostname and remove it and the space from the raw string
	hostname = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	return username + "@" + hostname;
}

string Message::parse_command()
{
	size_t pos = raw.find(' ');
	
	cmd = raw.substr(0, pos);
	
	// Either erase the rest of the string or the command and the space
	raw.erase(0, pos == string::npos ? pos : pos + 1);
	
	return cmd;
}

string Message::parse_arguments()
{
	// Check if we've already parsed the arguments
    if(!raw.empty())
    {
		// Parse the argument(s) from the msg string
		while(!raw.empty())
		{
		    size_t current_pos = raw.find(" ");
		    
		    // Check if a space character wasn't found
		    if(current_pos == string::npos)
		    {
				args.push_back(raw.substr(raw[0] == ':' ? 1 : 0));
				return args.back();
		    }

		    // Space was found so add the last "word" as an argument,
		    args.push_back(raw.substr(0, current_pos));
		    
		    // Now remove that last argument from the msg string,
		    // and increment the current_pos to skip the space character.
		    raw.erase(0, current_pos++);
		}
	}
	
	return args.back();
}

string Message::strip_end_newline(string line)
{
    return line.erase(line.find("\r\n"), 2);
}

unsigned int Message::get_reply_code()
{
    string reply_code_str = raw.substr(raw.find(" "), 4);
    unsigned int reply_code = 0;

    if (stringstream(reply_code_str) >> reply_code) {
        return reply_code;
    }

    return 0;
}

string Message::get_nickname()
{
    return hostname.substr(0, hostname.find("!"));
}

string Message::get_command()
{
    return cmd;
}

vector<string> Message::get_command_args()
{
	return args;
}
