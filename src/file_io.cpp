#include "file_io.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "bit_io.h"
#include "lzw.h"

constexpr int INITIAL_CODE_WIDTH = 9;
constexpr int MAX_CODE_WIDTH = 16;
constexpr int MAX_CODE = 65535;

std::string readBinaryFile(const std::string &path) {
  std::ifstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("Error opening file: " + path);
  }

  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

void writeBinaryFile(const std::string &path, const std::string &data) {
  std::ofstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("Error when creating new file: " + path);
  }

  file.write(data.data(), data.size());
}

void writeCompressedFile(const std::string &path,
                         const std::vector<int> &codes) {
  std::ofstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("Error creating compressed file: " + path);
  }

  const char header[4] = {'L', 'Z', 'W', '2'};
  file.write(header, 4);

  if (codes.empty()) {
    return;
  }

  BitWriter writer(file);

  int width = INITIAL_CODE_WIDTH;
  int nextCode = 256;

  writer.write(static_cast<unsigned int>(codes[0]), width);

  for (size_t i = 1; i < codes.size(); ++i) {
    if (nextCode == (1 << width) && width < MAX_CODE_WIDTH) {
      ++width;
    }

    int code = codes[i];

    if (code < 0 || code > MAX_CODE) {
      throw std::runtime_error("LZW code outside supported range");
    }

    writer.write(static_cast<unsigned int>(code), width);

    if (nextCode <= MAX_CODE) {
      ++nextCode;
    }
  }

  writer.flush();
}

std::vector<int> readCompressedFile(const std::string &path) {
  std::ifstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("Error reading compressed file: " + path);
  }

  char header[4];
  file.read(header, 4);

  if (file.gcount() != 4 || header[0] != 'L' || header[1] != 'Z' ||
      header[2] != 'W' || header[3] != '2') {
    throw std::runtime_error("Invalid LZW file format");
  }

  std::vector<int> codes;
  BitReader reader(file);

  int width = INITIAL_CODE_WIDTH;
  int nextCode = 256;

  unsigned int code;

  if (!reader.read(code, width)) {
    return codes;
  }

  codes.push_back(static_cast<int>(code));

  while (true) {
    if (nextCode == (1 << width) && width < MAX_CODE_WIDTH) {
      ++width;
    }

    if (!reader.read(code, width)) {
      break;
    }

    codes.push_back(static_cast<int>(code));

    if (nextCode <= MAX_CODE) {
      ++nextCode;
    }
  }

  return codes;
}

std::size_t compressFile(const std::string &inputPath,
                         const std::string &outputPath) {
  std::ifstream input(inputPath, std::ios::binary);

  if (!input) {
    throw std::runtime_error("Error opening file: " + inputPath);
  }

  std::ofstream output(outputPath, std::ios::binary);

  if (!output) {
    throw std::runtime_error("Error creating compressed file: " + outputPath);
  }

  const char header[4] = {'L', 'Z', 'W', '2'};
  output.write(header, 4);

  return compressStream(input, output);
}

void decompressFile(const std::string &inputPath,
                    const std::string &outputPath) {
  std::ifstream input(inputPath, std::ios::binary);

  if (!input) {
    throw std::runtime_error("Error reading compressed file: " + inputPath);
  }

  char header[4];
  input.read(header, 4);

  if (input.gcount() != 4 || header[0] != 'L' || header[1] != 'Z' ||
      header[2] != 'W' || header[3] != '2') {
    throw std::runtime_error("Invalid LZW file format");
  }

  std::ofstream output(outputPath, std::ios::binary);

  if (!output) {
    throw std::runtime_error("Error creating file: " + outputPath);
  }

  decompressStream(input, output);
}
