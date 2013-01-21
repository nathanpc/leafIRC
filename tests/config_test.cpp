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