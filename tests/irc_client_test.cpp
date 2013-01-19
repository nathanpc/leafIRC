// irc_client_test.cpp
// Test the different methods provided by the IRC_Client class.
#include "gtest/gtest.h"
#include "irc_client.h"

class IRC_ClientTest : public testing::Test
{
protected:
	// This will be called before each test is run.
	virtual void Setup()
	{
		client = IRC_Client("localhost");
	}
	
	// This will be called after each test is run.
	virtual void TearDown()
	{
	
	}
	
	// The IRC_Client we are going to test
	IRC_Client client;
};
