/**
 *  message.cpp
 *
 *  @author Nathan Campos
 */

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "message.h"
using namespace std;

Message::Message(const char* s)
{
	if((raw = _raw = strip_end_newline(s)).empty())
	{
		cerr << "Message::Message() \"message is empty\"\n";
		exit(0);
	}
	
	parse();
}

static string changeNewlines(const string& s)
{
	if(s.empty())
	{
		return s;
	}
	
	string retval = s;
	size_t i, sz;
	
	// Loop through the given string, changing any \r or \n
	for(i = 0, sz = retval.size(); i < sz - 1; ++i)
	{
		if(retval[i] == '\r' && i + 1 < sz && retval[i + 1] == '\n')
		{
			retval[i] = '\\';
			retval[i + 1] = 'n';
		}
	}
	
	return retval;
}

ostream& operator<<(ostream& out, const Message& m)
{
#ifdef DEBUG
	out << "\nraw = " << changeNewlines(m._raw) << "\n";
	out << "server = \"" << changeNewlines(m.server) << "\"\tusername = \""
		<< changeNewlines(m.username) << "\"\thostname = \""
		<< changeNewlines(m.hostname) << "\"\tcommand = \""
		<< changeNewlines(m.cmd) << "\"\narguments: ";
#endif
		
	if(m.args.empty())
	{
		out << "none\n";
	}
	else
	{
		size_t i, sz;
		
		for(i = 0, sz = m.args.size(); i < sz - 1; ++i)
		{
			out << "\"" << changeNewlines(m.args[i]) << "\", ";
		}
		
		// 'i' should be less than 'sz' but check anyways
		if(i < sz)
		{
			out << "\"" << changeNewlines(m.args[i]) << "\"\n";
		}
	}
	
	return out;
}

bool Message::parse()
{
	// Check when parsing for the server, if the message begins with NOTICE
	if(parse_server())
	{
		parse_username();
		parse_hostname();
	}

	parse_command();
	parse_arguments();
	
	return true;
}

bool Message::parse_server()
{
	// Validate the raw string before we start parsing.
	// It must not be empty and the first char should be a colon ':'
	if(raw.empty() || raw[0] != ':')
	{
		return false;
	}
	
	size_t exclPos, pos;
	
	// Get the index for the exclamation mark and check if it's not found
	if((exclPos = raw.find('!')) == string::npos)
	{
		// Look for a space instead
		if((pos = raw.find(' ')) != string::npos)
		{
			// Save the server/nick name from the raw string
			server = raw.substr(0, pos);
			
			// Remove the server/nick name plus space char from the raw string
			raw.erase(0, pos + 1);
			
			return false;
		}
		else
		{
			cerr << "Message::parse_server ~ \"invalid message\"\n";
			exit(EXIT_FAILURE);
		}
	}
	
	// Save the server/nick name from the raw string
	server = raw.substr(0, exclPos);

	// Remove the server/nick name plus exclamation mark from the raw string
	raw.erase(0, exclPos + 1);
	
	return true;
}

string Message::parse_username()
{
	size_t pos;
	
	// Look for the at symbol, if not found return an empty string
	if((pos = raw.find('@')) == string::npos)
	{
		return string();
	}
	
	// Save the username and remove it and the at symbol from the raw string
	username = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	return username;
}

string Message::parse_hostname()
{
	size_t pos;
	
	// Get the index for the first space character and check if it is not found
	if((pos = raw.find(' ')) == string::npos)
	{
		return string();
	}
	
	// Save the hostname and remove it and the space from the raw string
	hostname = raw.substr(0, pos);
	raw.erase(0, ++pos);
	
	return hostname;
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
		    size_t current_pos;
		    
		    // First check if this is the last "colon" argument
		    if(raw[0] == ':')
		    {
		    	// Push the last argument onto vector and erase the raw string
		    	args.push_back(raw.substr(1));
		    	raw.clear();
				return args.back();
		    }
		    
		    // We don't need to check if it was found or not
		    current_pos = raw.find(' ');
		    
		    // Add the last parsed argument to the arguments vector
		    args.push_back(raw.substr(0, current_pos));
		    
		    // Erase the last parsed argument from the raw string, possibly making it empty
		    raw.erase(0, current_pos == string::npos ? current_pos : current_pos + 1);
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

string Message::get_server()
{
	return server;
}

string Message::get_username()
{
	return username;
}

string Message::get_hostname()
{
	return hostname;
}

string Message::get_command()
{
    return cmd;
}

vector<string> Message::get_command_args()
{
	return args;
}
