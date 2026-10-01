#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "../src/lzw.h"

// Tests that empty input produces no codes and decompresses correctly.
TEST(LZW, EmptyInput) {
  std::string input = "";

  std::vector<int> codes = compress(input);

  EXPECT_TRUE(codes.empty());
  EXPECT_EQ(decompress(codes), input);
}

// Tests the simplest possible non-empty input.
TEST(LZW, SingleCharacter) {
  std::string input = "A";

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests highly repetitive input.
// This also exercises LZW's special decoder case where a code may refer
// to the next dictionary entry before it has explicitly been created.
TEST(LZW, RepeatedCharacters) {
  std::string input = "AAAAAAAAAAAAAAAAAAAA";

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests a known input against its expected LZW codes.
TEST(LZW, KnownABABABAEncoding) {
  std::string input = "ABABABA";

  std::vector<int> expected = {65, 66, 256, 258};

  EXPECT_EQ(compress(input), expected);
}

// Tests the complete round trip for the known ABABABA example.
TEST(LZW, ABABABARoundTrip) {
  std::string input = "ABABABA";

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests normal text containing spaces and punctuation.
TEST(LZW, NormalSentence) {
  std::string input = "The quick brown fox jumps over the lazy dog.";

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests repeated patterns that LZW should add to its dictionary.
TEST(LZW, RepeatedPattern) {
  std::string input = "ABCABCABCABCABCABCABCABCABCABC";

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests whitespace and special characters.
TEST(LZW, SpecialCharacters) {
  std::string input = "hello\nworld\t123!@#$%^&*()";

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests all possible byte values rather than only normal text.
TEST(LZW, AllByteValues) {
  std::string input;

  for (int i = 0; i < 256; ++i) {
    input += static_cast<char>(i);
  }

  EXPECT_EQ(decompress(compress(input)), input);
}

// Tests a larger input to exercise dictionary growth.
TEST(LZW, LargeRepeatedInput) {
  std::string input;

  for (int i = 0; i < 10000; ++i) {
    input += "ABCABCABCABC";
  }

  EXPECT_EQ(decompress(compress(input)), input);
}

// Invalid first code should not be accepted.
TEST(LZW, InvalidFirstCodeThrows) {
  std::vector<int> codes = {9999};

  EXPECT_THROW(decompress(codes), std::runtime_error);
}

// Invalid code appearing later should also be rejected.
TEST(LZW, InvalidCodeThrows) {
  std::vector<int> codes = {65, 9999};

  EXPECT_THROW(decompress(codes), std::runtime_error);
}
