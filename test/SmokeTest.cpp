/* Smoke Tests
 * Simple Tests for Midwest Market Machines
 * The Simple Trade App
 *
 * Hunter William Poole
 * 2-25-2026
 *
 * SmokeTest.cpp
 * This test suite is presented as a prototype of testing methods for any C++
 * services used. Currently, it tests that tests works (you may run this program
 * as a test and 1 == 1), and that we are able to build a JSON object then
 * serialize it to known output.
 *
 * Both of the major imports for this project, IXWebSockets and nlohmannjson
 * have their own test suites that are being pulled from their respective GitHub
 * repositories and included in this application.
 *
 * Later, it would be nice if we could run their test suites in parallel to our
 * own.
 */

#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// TODO: Figure out how to build the tested items once, outside of the test
// itself. Would be useful as test files grow in size and quantity.

/*
class JSONTest : public testing::Test {
protected:
  void OnePairTest() {
    TestingJSON = {
        {"Item", 7},
    };
  }

  json TestingJSON;
};
*/

// Filler - In a way is testing that tests work.
TEST(dummytest, test) { EXPECT_EQ(1, 1); }

// Can we construct a JSON object with known inputs, and serialize it to known
// output?
TEST(JSONSuite, OnePairTest) {
  json TestingJSON1 = {{"Item", 7}};
  json TestingJSON2;
  TestingJSON2["Item"] = 2;

  EXPECT_TRUE(TestingJSON1.dump() == "{\"Item\":7}");
  EXPECT_TRUE(TestingJSON2.dump() == "{\"Item\":2}");
}
