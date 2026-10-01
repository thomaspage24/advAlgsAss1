#include "bit_io.h"

#include <stdexcept>

BitWriter::BitWriter(std::ostream &output) : output(output) {}

void BitWriter::write(std::uint32_t value, int width) {
  if (width <= 0 || width > 32) {
    throw std::runtime_error("Invalid bit width");
  }

  buffer |= static_cast<std::uint64_t>(value) << bitCount;
  bitCount += width;

  while (bitCount >= 8) {
    output.put(static_cast<char>(buffer & 0xFF));

    buffer >>= 8;
    bitCount -= 8;
  }
}

void BitWriter::flush() {
  if (bitCount > 0) {
    output.put(static_cast<char>(buffer & 0xFF));

    buffer = 0;
    bitCount = 0;
  }
}

BitReader::BitReader(std::istream &input) : input(input) {}

bool BitReader::read(std::uint32_t &value, int width) {
  if (width <= 0 || width > 32) {
    throw std::runtime_error("Invalid bit width");
  }

  while (bitCount < width) {
    int byte = input.get();

    if (byte == EOF) {
      return false;
    }

    buffer |= static_cast<std::uint64_t>(static_cast<unsigned char>(byte))
              << bitCount;

    bitCount += 8;
  }

  std::uint64_t mask = (static_cast<std::uint64_t>(1) << width) - 1;

  value = static_cast<std::uint32_t>(buffer & mask);

  buffer >>= width;
  bitCount -= width;

  return true;
}
