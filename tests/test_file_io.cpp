#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../src/file_io.h"

// Creates temporary file paths so tests do not interfere
// with the normal samples directory.
std::string tempFile(const std::string& name) {
  return (std::filesystem::temp_directory_path() / name).string();
}

// Tests normal binary file writing and reading.
TEST(FileIO, BinaryFileRoundTrip) {
  std::string path = tempFile("lzw_binary_test.bin");

  std::string original = "Hello world\nABCABCABC\n123456789";

  writeBinaryFile(path, original);

  std::string restored = readBinaryFile(path);

  EXPECT_EQ(restored, original);

  std::filesystem::remove(path);
}

// Ensures binary files containing null bytes are handled correctly.
TEST(FileIO, BinaryBytesRoundTrip) {
  std::string path = tempFile("lzw_bytes_test.bin");

  std::string original;

  for (int i = 0; i < 256; ++i) {
    original += static_cast<char>(i);
  }

  writeBinaryFile(path, original);

  std::string restored = readBinaryFile(path);

  EXPECT_EQ(restored, original);

  std::filesystem::remove(path);
}

// Tests writing packed LZW codes and reading them back.
TEST(FileIO, CompressedCodesRoundTrip) {
  std::string path = tempFile("lzw_codes_test.lzw");

  std::vector<int> original = {65, 66, 256, 258};

  writeCompressedFile(path, original);

  std::vector<int> restored = readCompressedFile(path);

  EXPECT_EQ(restored, original);

  std::filesystem::remove(path);
}

// Tests a larger set of codes so bit widths eventually increase.
TEST(FileIO, LargeCompressedCodesRoundTrip) {
  std::string path = tempFile("lzw_large_codes_test.lzw");

  std::vector<int> original;

  for (int i = 0; i < 2000; ++i) {
    original.push_back(i % 256);
  }

  writeCompressedFile(path, original);

  std::vector<int> restored = readCompressedFile(path);

  EXPECT_EQ(restored, original);

  std::filesystem::remove(path);
}

// Checks that files written by the compressor use the LZW2 header.
TEST(FileIO, LZW2Header) {
  std::string path = tempFile("lzw_header_test.lzw");

  std::vector<int> codes = {65, 66, 256};

  writeCompressedFile(path, codes);

  std::ifstream file(path, std::ios::binary);

  char header[4];
  file.read(header, 4);

  EXPECT_EQ(header[0], 'L');
  EXPECT_EQ(header[1], 'Z');
  EXPECT_EQ(header[2], 'W');
  EXPECT_EQ(header[3], '2');

  std::filesystem::remove(path);
}

// Ensures files without the correct LZW2 header are rejected.
TEST(FileIO, InvalidHeaderThrows) {
  std::string path = tempFile("lzw_invalid_header.lzw");

  {
    std::ofstream file(path, std::ios::binary);

    const char header[4] = {'B', 'A', 'D', '!'};

    file.write(header, 4);
  }

  EXPECT_THROW(readCompressedFile(path), std::runtime_error);

  std::filesystem::remove(path);
}

// Empty input should still produce a valid LZW2 file.
TEST(FileIO, EmptyCompressedFile) {
  std::string path = tempFile("lzw_empty_test.lzw");

  std::vector<int> original;

  writeCompressedFile(path, original);

  std::vector<int> restored = readCompressedFile(path);

  EXPECT_TRUE(restored.empty());

  std::filesystem::remove(path);
}

// Attempting to read a file that does not exist should fail.
TEST(FileIO, MissingFileThrows) {
  std::string path = tempFile("this_file_should_not_exist_123456.lzw");

  std::filesystem::remove(path);

  EXPECT_THROW(readCompressedFile(path), std::runtime_error);
}
