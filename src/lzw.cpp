#include "lzw.h"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "bit_io.h"

constexpr int MAX_CODE = 65535;
constexpr int INITIAL_CODE_WIDTH = 9;
constexpr int MAX_CODE_WIDTH = 16;

std::vector<int> compress(const std::string &input) {
  if (input.empty()) {
    return {};
  }

  std::unordered_map<std::string, int> dict;

  for (std::size_t i = 0; i < 256; ++i) {
    dict[std::string(1, static_cast<char>(i))] = i;
  }

  int nextCode = 256;

  std::vector<int> output;

  // alg to compress
  std::string curr;

  for (char c : input) {
    std::string candidate = curr + c;
    if (dict.contains(candidate)) {
      curr = candidate;
    } else {
      output.push_back(dict.at(curr));
      if (nextCode <= MAX_CODE) {
        dict[candidate] = nextCode;
        ++nextCode;
      }
      curr = std::string(1, c);
    }
  }
  if (!curr.empty()) {
    output.push_back(dict.at(curr));
  }

  return output;
}

std::string decompress(const std::vector<int> &codes) {
  if (codes.empty()) {
    return "";
  }

  std::unordered_map<int, std::string> dict;

  for (std::size_t i = 0; i < 256; ++i) {
    dict[i] = std::string(1, static_cast<char>(i));
  }

  int nextCode = 256;
  if (!dict.contains(codes[0])) {
    throw std::runtime_error("invalid lzw header code");
  }
  std::string prev = dict.at(codes[0]);
  std::string output = prev;

  for (size_t i = 1; i < codes.size(); ++i) {
    int code = codes[i];
    std::string curr;
    if (dict.contains(code)) {
      curr = dict.at(code);
    } else if (code == nextCode && nextCode <= MAX_CODE) {
      curr = prev + prev[0];
    } else {
      throw std::runtime_error("invalid lzw codes");
    }

    output += curr;

    if (nextCode <= MAX_CODE) {
      dict[nextCode] = prev + curr[0];
      ++nextCode;
    }
    prev = curr;
  }

  return output;
}

std::size_t compressStream(std::istream &input, std::ostream &output) {
  std::unordered_map<std::string, int> dict;

  for (std::size_t i = 0; i < 256; ++i) {
    dict[std::string(1, static_cast<char>(i))] = static_cast<int>(i);
  }

  int nextCode = 256;

  BitWriter writer(output);

  int width = INITIAL_CODE_WIDTH;
  int packNextCode = 256;

  bool firstCode = true;
  std::size_t codeCount = 0;

  auto writeCode = [&](int code) {
    if (!firstCode) {
      if (packNextCode == (1 << width) && width < MAX_CODE_WIDTH) {
        ++width;
      }

      if (packNextCode <= MAX_CODE) {
        ++packNextCode;
      }
    }

    writer.write(static_cast<std::uint32_t>(code), width);

    firstCode = false;
    ++codeCount;
  };

  std::string curr;
  char c;

  while (input.get(c)) {
    std::string candidate = curr + c;

    if (dict.contains(candidate)) {
      curr = candidate;
    } else {
      writeCode(dict.at(curr));

      if (nextCode <= MAX_CODE) {
        dict[candidate] = nextCode;
        ++nextCode;
      }

      curr = std::string(1, c);
    }
  }

  if (!curr.empty()) {
    writeCode(dict.at(curr));
  }

  writer.flush();

  return codeCount;
}

void decompressStream(std::istream &input, std::ostream &output) {
  std::unordered_map<int, std::string> dict;

  for (std::size_t i = 0; i < 256; ++i) {
    dict[static_cast<int>(i)] = std::string(1, static_cast<char>(i));
  }

  BitReader reader(input);

  int nextCode = 256;

  int width = INITIAL_CODE_WIDTH;
  int packNextCode = 256;

  std::uint32_t code;

  if (!reader.read(code, width)) {
    return;
  }

  if (!dict.contains(static_cast<int>(code))) {
    throw std::runtime_error("invalid first lzw code");
  }

  std::string prev = dict.at(static_cast<int>(code));

  output.write(prev.data(), prev.size());

  while (true) {
    if (packNextCode == (1 << width) && width < MAX_CODE_WIDTH) {
      ++width;
    }

    if (!reader.read(code, width)) {
      break;
    }

    if (packNextCode <= MAX_CODE) {
      ++packNextCode;
    }

    int currentCode = static_cast<int>(code);

    std::string curr;

    if (dict.contains(currentCode)) {
      curr = dict.at(currentCode);
    } else if (currentCode == nextCode && nextCode <= MAX_CODE) {
      curr = prev + prev[0];
    } else {
      throw std::runtime_error("invalid lzw codes");
    }

    output.write(curr.data(), curr.size());

    if (nextCode <= MAX_CODE) {
      dict[nextCode] = prev + curr[0];
      ++nextCode;
    }

    prev = curr;
  }
}
