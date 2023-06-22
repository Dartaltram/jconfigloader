//COMPILER: g++ -std=c++17 Test.cpp -lgtest -lgtest_main -pthread ../jconfigloader.cpp -o test_jconfigloader

#include <iostream>
#include "../jconfigloader.h"

#include <gtest/gtest.h>

#define TEST_DEFAULT_FILE "test_defaultdata.json"   //used to test the creation of a default data file
#define TEST_DATA_FILE "test_data.json"             //used to test the functionalities of the .json file

TEST(ConstructorTests, EmptyPath) {

    //DEBUG: testing constructor with empty path
    jconfigloader::parameterObj* myParam = nullptr;

    ASSERT_THROW(myParam = new jconfigloader::parameterObj(""), std::runtime_error);
    ASSERT_TRUE(myParam == nullptr);
}
TEST(ConstructorTests, WrongPath_NoData) {

    //DEBUG: testing constructor with garbage path and no data
    jconfigloader::parameterObj* myParam = nullptr;

    ASSERT_THROW(myParam = new jconfigloader::parameterObj("somefile.json", nullptr, 0), std::runtime_error);
    ASSERT_TRUE(myParam == nullptr);
}
TEST(ConstructorTests, NoPath_DefaultData) {

    //DEBUG: testing constructor with valid but NOT EXISTING path and default data

    //remove file if it exists so it doesn't impair other tests
    try {
        if (std::filesystem::exists(TEST_DEFAULT_FILE))
            std::filesystem::remove(TEST_DEFAULT_FILE);
    } catch (const std::filesystem::filesystem_error& e) {
        ASSERT_FALSE(true);
    }

    std::string defaultLoggerParam[] = {

        "{\n",
        "\t\"Version\" : \"1.0\",\n",
        "\n",
        "\t\"First_Object\" : {\n",
        "\t\t\"fo_firstParam\" : true,\n",
        "\t\t\"fo_secondParam\" : 5,\n",
        "\t\t\"fo_thirdParam\" : \"hello world\"\n",
        "\t\t},\n",
        "\n",
        "\t\"Second_Object\" : {\n",
        "\t\t\"so_firstParam\" : false,\n",
        "\t\t\"so_secondParam\" : 0,\n",
        "\t\t\"so_thirdParam\" : \"goodbye world\"\n",
        "\t}\n",
        "}"
    };

    size_t length = sizeof(defaultLoggerParam) / sizeof(defaultLoggerParam[0]);

    jconfigloader::parameterObj* myParam = nullptr;
    myParam = new jconfigloader::parameterObj(TEST_DEFAULT_FILE, defaultLoggerParam, length);

    ASSERT_TRUE(myParam != nullptr);                //check if paramObj is formed
    ASSERT_EQ(myParam->file(), TEST_DEFAULT_FILE);  //check if filepath is correctly saved
    ASSERT_TRUE(myParam->data()->is_object());      //check if data is loaded correctly

    //remove the file if it exists
    if (std::filesystem::exists(TEST_DEFAULT_FILE))
        std::filesystem::remove(TEST_DEFAULT_FILE);
}
TEST(ConstructorTests, CorrectPath) {

    //DEBUG: test with a valid EXISTING path
    // data is not important as it only comes into play if the file doesn't exist

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    ASSERT_TRUE(myParam != nullptr);            //check if paramObj is formed
    ASSERT_EQ(myParam->file(), TEST_DATA_FILE); //check if filepath is correctly saved
    ASSERT_TRUE(myParam->data()->is_object());  //check if data is loaded correctly
}
TEST(ConstructorTests, Filter_layer0) {

    //DEBUG: test with no filter

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE, {});

    EXPECT_TRUE(myParam->data()->contains("Version"));
    EXPECT_TRUE(myParam->data()->contains("First_Object"));
    EXPECT_TRUE(myParam->data()->contains("Second_Object"));
}
TEST(ConstructorTests, Filter_layer1_single) {

    //DEBUG: test with 1 layer filter - single entry
    // deletes the whole param object

    jconfigloader::parameterObj* myParam = nullptr;
    ASSERT_THROW( myParam = new jconfigloader::parameterObj(TEST_DATA_FILE, {"Version"}), std::logic_error);

    ASSERT_TRUE(myParam == nullptr);            //check if paramObj is formed
}
TEST(ConstructorTests, Filter_layer1_object) {

    //DEBUG: test with 1 layer filter - complex object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE, {"First_Object"});

    EXPECT_FALSE(myParam->data()->contains("Version"));
    EXPECT_FALSE(myParam->data()->contains("First_Object"));
    EXPECT_TRUE(myParam->data()->contains("fo_firstParam"));
    EXPECT_TRUE(myParam->data()->contains("fo_secondParam"));
    EXPECT_TRUE(myParam->data()->contains("fo_thirdParam"));
    EXPECT_FALSE(myParam->data()->contains("Second_Object"));
}
TEST(ConstructorTests, Filter_layer2_object) {

    //DEBUG: test with 2 layer filter - complex object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE, {"Second_Object", "so_fourthParam"});

    EXPECT_FALSE(myParam->data()->contains("Version"));
    EXPECT_FALSE(myParam->data()->contains("First_Object"));
    EXPECT_FALSE(myParam->data()->contains("Second_Object"));
    EXPECT_FALSE(myParam->data()->contains("so_fourthParam"));
    EXPECT_TRUE(myParam->data()->contains("so_fp_Iparam"));
    EXPECT_TRUE(myParam->data()->contains("so_fp_IIparam"));
    EXPECT_TRUE(myParam->data()->contains("so_fp_IIIparam"));
}

TEST(PrintTests, AllKeys) {

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    ASSERT_NO_FATAL_FAILURE(myParam->print_keys());
}
TEST(PrintTests, FilteredKeys) {

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE, {"Second_Object"});

    ASSERT_NO_FATAL_FAILURE(myParam->print_keys());
}

TEST(HasKeyTests, AllKeys) {

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    ASSERT_TRUE(myParam->has_key({"Version"}));
    ASSERT_TRUE(myParam->has_key({"First_Object"}));
    ASSERT_TRUE(myParam->has_key({"Second_Object"}));
    ASSERT_TRUE(myParam->has_key({"Second_Object", "so_firstParam"}));

}
TEST(HasKeyTests, FilteredKeys) {

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE, {"Second_Object"});

    ASSERT_FALSE(myParam->has_key({"Version"}));
    ASSERT_FALSE(myParam->has_key({"First_Object"}));
    ASSERT_FALSE(myParam->has_key({"Second_Object"}));
    ASSERT_TRUE(myParam->has_key({"so_firstParam"}));
    ASSERT_TRUE(myParam->has_key({"so_secondParam"}));
    ASSERT_TRUE(myParam->has_key({"so_fourthParam", "so_fp_IIparam"}));
}

TEST(GetValueTests, GetBool) {

    //DEBUG: get a bool value from the param object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    ASSERT_TRUE(myParam->get_value<bool>({"boolParam"}));
    ASSERT_TRUE(myParam->get_value<bool>({"First_Object", "fo_firstParam"}));
    ASSERT_TRUE(myParam->get_value<bool>({"Second_Object", "so_fourthParam", "so_fp_IIparam"}));
}
TEST(GetValueTests, GetString) {

    //DEBUG: get a bool value from the param object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    std::string testVal[] = {
        myParam->get_value<string>({"Version"}),
        myParam->get_value<string>({"First_Object", "fo_thirdParam"}),
        myParam->get_value<string>({"Second_Object", "so_fourthParam", "so_fp_IIIparam"}),
    };

    ASSERT_EQ(testVal[0], "1.0");
    ASSERT_EQ(testVal[1], "hello world");
    ASSERT_EQ(testVal[2], "3rd layer");
}
TEST(GetValueTests, GetNumber_int) {

    //DEBUG: get a bool value from the param object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    int testVal[] = {
        myParam->get_value<int>({"intParam"}),
        myParam->get_value<int>({"First_Object", "fo_secondParam"}),
        myParam->get_value<int>({"Second_Object", "so_fourthParam", "so_fp_Iparam"}),
    };

    ASSERT_EQ(testVal[0], 5);
    ASSERT_EQ(testVal[1], 5);
    ASSERT_EQ(testVal[2], 55);
}
TEST(GetValueTests, GetNumber_float) {

    //DEBUG: get a bool value from the param object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);

    float testVal[] = {
        myParam->get_value<float>({"floatParam"}),
        myParam->get_value<float>({"Second_Object", "so_fourthParam", "so_fp_Iparam"}),
    };

    ASSERT_EQ(testVal[0], 2.5f);
    ASSERT_EQ(testVal[1], 55.24f);
}
//TODO: still to be implemented
/*TEST(GetValueTests, GetNull) {

    //DEBUG: get a bool value from the param object

    jconfigloader::parameterObj* myParam = new jconfigloader::parameterObj(TEST_DATA_FILE);
}*/

int main(int argc, char** argv) {

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}