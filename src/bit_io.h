#ifndef BIT_IO_H
#define BIT_IO_H

#include <cstdint>
#include <istream>
#include <ostream>

class BitWriter {
 private:
  std::ostream& output;
  std::uint64_t buffer = 0;
  int bitCount = 0;

 public:
  explicit BitWriter(std::ostream& output);

  void write(std::uint32_t value, int width);

  void flush();
};

class BitReader {
 private:
  std::istream& input;
  std::uint64_t buffer = 0;
  int bitCount = 0;

 public:
  explicit BitReader(std::istream& input);

  bool read(std::uint32_t& value, int width);
};

#endif
