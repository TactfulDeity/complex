#include "simplnx/Utilities/StringUtilities.hpp"
#include "simplnx/unit_test/simplnx_test_dirs.hpp"

#include <catch2/catch.hpp>

#include <string>

using namespace nx::core;

TEST_CASE("Simplnx::StringUtilities::Split Test", "[Simplnx][StringUtilities]")
{
  std::string emptyInput = "";
  std::string noDelimiterInput = "ThisIsABaselineTest";

  std::vector<std::string> result;
  std::array<char, 1> k_Delimiter = {'|'};

  // Case 1
  result = StringUtilities::split(emptyInput, k_Delimiter[0]);

  REQUIRE(result.empty());

  result = StringUtilities::split(noDelimiterInput, k_Delimiter[0]);

  REQUIRE(result == std::vector<std::string>{noDelimiterInput});

  // Case 2
  result = StringUtilities::split(emptyInput, k_Delimiter, true);

  REQUIRE(result.empty());

  result = StringUtilities::split(noDelimiterInput, k_Delimiter, true);

  REQUIRE(result == std::vector<std::string>{noDelimiterInput});

  // Case 4
  result = StringUtilities::specific_split(emptyInput, k_Delimiter, StringUtilities::SplitType::OnlyConsecutive);

  REQUIRE(result.empty());

  result = StringUtilities::specific_split(noDelimiterInput, k_Delimiter, StringUtilities::SplitType::OnlyConsecutive);

  REQUIRE(result == std::vector<std::string>{noDelimiterInput});

  // Case 5
  result = StringUtilities::specific_split(emptyInput, k_Delimiter, StringUtilities::SplitType::NoStripIgnoreConsecutive);

  REQUIRE(result.empty());

  result = StringUtilities::specific_split(noDelimiterInput, k_Delimiter, StringUtilities::SplitType::NoStripIgnoreConsecutive);

  REQUIRE(result == std::vector<std::string>{noDelimiterInput});

  // Case 6
  result = StringUtilities::specific_split(emptyInput, k_Delimiter, StringUtilities::SplitType::AllowEmptyLeftAnalyze);

  REQUIRE(result.empty());

  result = StringUtilities::specific_split(noDelimiterInput, k_Delimiter, StringUtilities::SplitType::AllowEmptyLeftAnalyze);

  REQUIRE(result == std::vector<std::string>{noDelimiterInput});

  // Case 7
  result = StringUtilities::specific_split(emptyInput, k_Delimiter, StringUtilities::SplitType::AllowEmptyRightAnalyze);

  REQUIRE(result.empty());

  result = StringUtilities::specific_split(noDelimiterInput, k_Delimiter, StringUtilities::SplitType::AllowEmptyRightAnalyze);

  REQUIRE(result == std::vector<std::string>{noDelimiterInput});
}

TEST_CASE("Simplnx::StringUtilities Utility Function Test1", "[Simplnx][StringUtilities]")
{
  // Case 1
  std::string inputStr = "This|Is|A|Baseline|Test";

  std::vector<std::string> result = StringUtilities::split(inputStr, '|');

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 2
  inputStr = "|This|Is|A|Baseline|Test|";

  result = StringUtilities::split(inputStr, '|');

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 3
  inputStr = "||This|Is||A||Baseline|Test||";

  result = StringUtilities::split(inputStr, '|');

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});
}

TEST_CASE("Simplnx::StringUtilities Utility Function Test2", "[Simplnx][StringUtilities]")
{
  std::array<char, 1> k_Delimiter = {'|'};
  // Case 1
  std::string inputStr = "This|Is|A|Baseline|Test";

  std::vector<std::string> result = StringUtilities::split(inputStr, k_Delimiter, false);

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 2
  inputStr = "|This|Is|A|Baseline|Test|";

  result = StringUtilities::split(inputStr, k_Delimiter, false);

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 3
  inputStr = "||This|Is||A||Baseline|Test||";

  result = StringUtilities::split(inputStr, k_Delimiter, false);

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 4
  inputStr = "This|Is|A|Baseline|Test";

  result = StringUtilities::split(inputStr, k_Delimiter, true);

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 5
  inputStr = "|This|Is|A|Baseline|Test|";

  result = StringUtilities::split(inputStr, k_Delimiter, true);

  REQUIRE(result == std::vector<std::string>{"", "This", "Is", "A", "Baseline", "Test", ""});

  // Case 6
  inputStr = "||This|Is||A||Baseline|Test||";

  result = StringUtilities::split(inputStr, k_Delimiter, true);

  REQUIRE(result == std::vector<std::string>{"", "", "This", "Is", "", "A", "", "Baseline", "Test", "", ""});
}

TEST_CASE("Simplnx::StringUtilities Utility Function Test3", "[Simplnx][StringUtilities]")
{
  std::array<char, 1> k_Delimiter = {'|'};
  std::string inputStr = "||This|Is||A||Baseline|Test||";
  std::vector<std::string> result;

  // Case 1
  result = StringUtilities::specific_split(inputStr, k_Delimiter, StringUtilities::SplitType::AllowAll);

  REQUIRE(result == std::vector<std::string>{"", "", "This", "Is", "", "A", "", "Baseline", "Test", "", ""});

  // Case 2
  result = StringUtilities::specific_split(inputStr, k_Delimiter, StringUtilities::SplitType::IgnoreEmpty);

  REQUIRE(result == std::vector<std::string>{"This", "Is", "A", "Baseline", "Test"});

  // Case 3
  result = StringUtilities::specific_split(inputStr, k_Delimiter, StringUtilities::SplitType::NoStripIgnoreConsecutive);

  REQUIRE(result == std::vector<std::string>{"", "This", "Is", "A", "Baseline", "Test", ""});

  // Case 4
  result = StringUtilities::specific_split(inputStr, k_Delimiter, StringUtilities::SplitType::OnlyConsecutive);

  REQUIRE(result == std::vector<std::string>{"", "This", "Is", "", "A", "", "Baseline", "Test", ""});

  // Case 5
  result = StringUtilities::specific_split(inputStr, k_Delimiter, StringUtilities::SplitType::AllowEmptyLeftAnalyze);

  REQUIRE(result == std::vector<std::string>{"", "", "This", "Is", "", "A", "", "Baseline", "Test", ""});

  // Case 5
  result = StringUtilities::specific_split(inputStr, k_Delimiter, StringUtilities::SplitType::AllowEmptyRightAnalyze);

  REQUIRE(result == std::vector<std::string>{"", "This", "Is", "", "A", "", "Baseline", "Test", "", ""});
}
