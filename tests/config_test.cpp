#include "gtest/gtest.h"
#include "config.h"
#include "locations.h"

class ConfigTest : public testing::Test {
    protected:
        ConfigTest() : config() {};

        Config config;
};

/*TEST_F(ConfigTest, Constructor) {
    
}*/

TEST_F(ConfigTest, CheckDirectories) {
    EXPECT_NO_THROW({
        config.check_dirs("history");
    });
}

TEST_F(ConfigTest, LoadUserAliasConfiguration) {
    ASSERT_NO_THROW({
        config.load_user_config("freenode");
    });
    
    EXPECT_EQ("irc.freenode.net", config.server_location);
    EXPECT_EQ("leafirc", config.user_nick);
    EXPECT_EQ("leafirc", config.user_username);
    EXPECT_EQ("LeafIRC", config.user_realname);
    //std::vector<std::string> channels;
}

TEST_F(ConfigTest, LoadUserNonAliasConfiguration) {
    ASSERT_NO_THROW({
        config.load_user_config("localhost");
    });

    EXPECT_EQ("localhost", config.server_location);
    EXPECT_EQ("leafirc", config.user_nick);
    EXPECT_EQ("leafirc", config.user_username);
    EXPECT_EQ("LeafIRC", config.user_realname);
    EXPECT_EQ("", config.user_password);
    //std::vector<std::string> channels;
}