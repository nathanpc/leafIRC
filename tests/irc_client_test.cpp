// irc_client_test.cpp
// Test the different methods provided by the IRC_Client class.
#include "gtest/gtest.h"
#include "irc_client.h"

class IRC_ClientTest : public testing::Test
{
protected:
	IRC_ClientTest() : client("localhost")
	{
		client.setup_user("Test", "TestName", "leafIRC");
	}
	
	// This will be called before each test is run.
	virtual void SetUp()
	{
		client.setup_user("Test", "TestName", "leafIRC");
	}
	
	// This will be called after each test is run.
//	virtual void TearDown()
//	{
//		
//	}
	
	// The IRC_Client we are going to test
	IRC_Client client;
};

// Test the constructor
TEST_F(IRC_ClientTest, Constructor)
{
	EXPECT_EQ("localhost", client.server);
	EXPECT_EQ("6667", client.port);
	EXPECT_EQ("", client.server_password);
}

// Test the setup_user() function
TEST_F(IRC_ClientTest, SetupUser)
{
	EXPECT_EQ("Test", client.nick);
	EXPECT_EQ("TestName", client.username);
	EXPECT_EQ("leafIRC", client.realname);
}

// Test the start_connection() and close_connection() functions
TEST_F(IRC_ClientTest, Connection)
{
	client.start_connection();
	ASSERT_EQ(true, client.is_connected());
	
	int retval = client.run();
	
	EXPECT_EQ(0, retval);
	EXPECT_EQ(false, client.is_connected());
	
	// Check for the command 001
	bool found = false;
	std::vector<Message> mvec;
	typedef std::vector<Message>::iterator LogIter;
	
	for(LogIter it = client.log.begin(); it != client.log.end(); ++it)
	{
		mvec.push_back(*it);
		
		if(it->get_command() == "001")
		{
			found = true;
			break;
		}
	}
	
	EXPECT_EQ(true, found);
}
