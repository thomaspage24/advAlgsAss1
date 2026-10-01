#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>

#include "../src/bit_io.h"

TEST(BitIO, NineBitValues) {
  std::stringstream stream;

  BitWriter writer(stream);

  writer.write(65, 9);
  writer.write(66, 9);
  writer.write(256, 9);
  writer.write(258, 9);

  writer.flush();

  BitReader reader(stream);

  std::uint32_t value;

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 65);

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 66);

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 256);

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 258);
}

TEST(BitIO, MixedWidths) {
  std::stringstream stream;

  BitWriter writer(stream);

  writer.write(511, 9);
  writer.write(512, 10);
  writer.write(1023, 10);
  writer.write(1024, 11);

  writer.flush();

  BitReader reader(stream);

  std::uint32_t value;

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 511);

  ASSERT_TRUE(reader.read(value, 10));
  EXPECT_EQ(value, 512);

  ASSERT_TRUE(reader.read(value, 10));
  EXPECT_EQ(value, 1023);

  ASSERT_TRUE(reader.read(value, 11));
  EXPECT_EQ(value, 1024);
}

TEST(BitIO, MinimumValues) {
  std::stringstream stream;

  BitWriter writer(stream);

  writer.write(0, 9);
  writer.write(1, 9);

  writer.flush();

  BitReader reader(stream);

  std::uint32_t value;

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 0);

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 1);
}

TEST(BitIO, MaximumSixteenBitValue) {
  std::stringstream stream;

  BitWriter writer(stream);

  writer.write(65535, 16);

  writer.flush();

  BitReader reader(stream);

  std::uint32_t value;

  ASSERT_TRUE(reader.read(value, 16));
  EXPECT_EQ(value, 65535);
}

TEST(BitIO, WidthBoundaries) {
  std::stringstream stream;

  BitWriter writer(stream);

  writer.write(511, 9);
  writer.write(512, 10);
  writer.write(1023, 10);
  writer.write(1024, 11);
  writer.write(2047, 11);
  writer.write(2048, 12);

  writer.flush();

  BitReader reader(stream);

  std::uint32_t value;

  ASSERT_TRUE(reader.read(value, 9));
  EXPECT_EQ(value, 511);

  ASSERT_TRUE(reader.read(value, 10));
  EXPECT_EQ(value, 512);

  ASSERT_TRUE(reader.read(value, 10));
  EXPECT_EQ(value, 1023);

  ASSERT_TRUE(reader.read(value, 11));
  EXPECT_EQ(value, 1024);

  ASSERT_TRUE(reader.read(value, 11));
  EXPECT_EQ(value, 2047);

  ASSERT_TRUE(reader.read(value, 12));
  EXPECT_EQ(value, 2048);
}

TEST(BitIO, InvalidWriteWidthThrows) {
  std::stringstream stream;

  BitWriter writer(stream);

  EXPECT_THROW(writer.write(10, 0), std::runtime_error);
  EXPECT_THROW(writer.write(10, 33), std::runtime_error);
}

TEST(BitIO, InvalidReadWidthThrows) {
  std::stringstream stream;

  BitReader reader(stream);

  std::uint32_t value;

  EXPECT_THROW(reader.read(value, 0), std::runtime_error);
  EXPECT_THROW(reader.read(value, 33), std::runtime_error);
}
