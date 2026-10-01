#include <chrono>
#include <cstddef>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "file_io.h"

// CLI colours
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string RED = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";

void printUsage() {
  std::cout << BOLD << "LZW Compressor\n" << RESET;

  std::cout << "\nUsage:\n";
  std::cout << "  " << CYAN << "./lzw compress <input> <output>" << RESET
            << '\n';

  std::cout << "  " << CYAN << "./lzw decompress <input> <output>" << RESET
            << '\n';

  std::cout << "\nExamples:\n";

  std::cout << "  ./lzw compress samples/test.txt samples/test.lzw\n";
  std::cout << "  ./lzw decompress samples/test.lzw samples/restored.txt\n";
}

std::string formatBytes(std::uintmax_t bytes) {
  const double KB = 1024.0;
  const double MB = KB * 1024.0;
  const double GB = MB * 1024.0;

  std::ostringstream output;

  output << std::fixed << std::setprecision(2);

  if (bytes >= GB) {
    output << static_cast<double>(bytes) / GB << " GB";
  } else if (bytes >= MB) {
    output << static_cast<double>(bytes) / MB << " MB";
  } else if (bytes >= KB) {
    output << static_cast<double>(bytes) / KB << " KB";
  } else {
    output << bytes << " B";
  }

  return output.str();
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    std::string option = argv[1];

    if (option == "--help" || option == "-h") {
      printUsage();
      return 0;
    }
  }

  if (argc != 4) {
    std::cerr << RED << "Error: incorrect number of arguments\n" << RESET;

    printUsage();
    return 1;
  }

  std::string command = argv[1];
  std::string inputPath = argv[2];
  std::string outputPath = argv[3];

  if (command != "compress" && command != "decompress") {
    std::cerr << RED << "Error: unknown command '" << command << "'\n" << RESET;

    std::cerr << "Use 'compress' or 'decompress'.\n";
    return 1;
  }

  if (inputPath == outputPath) {
    std::cerr << RED << "Error: input and output files must be different\n"
              << RESET;

    return 1;
  }

  if (!std::filesystem::exists(inputPath)) {
    std::cerr << RED << "Error: input file does not exist: " << inputPath
              << '\n'
              << RESET;

    return 1;
  }

  try {
    auto start = std::chrono::steady_clock::now();

    if (command == "compress") {
      std::cout << CYAN << "Compressing " << RESET << inputPath << "...\n";

      std::size_t codeCount = compressFile(inputPath, outputPath);

      std::uintmax_t originalSize = std::filesystem::file_size(inputPath);

      std::uintmax_t compressedSize = std::filesystem::file_size(outputPath);

      long long bytesSaved = static_cast<long long>(originalSize) -
                             static_cast<long long>(compressedSize);

      double compressionRatio = 0.0;
      double reduction = 0.0;

      if (originalSize > 0 && compressedSize > 0) {
        compressionRatio = static_cast<double>(originalSize) /
                           static_cast<double>(compressedSize);

        reduction = (1.0 - static_cast<double>(compressedSize) /
                               static_cast<double>(originalSize)) *
                    100.0;
      }

      auto end = std::chrono::steady_clock::now();

      double seconds = std::chrono::duration<double>(end - start).count();

      std::cout << '\n'
                << GREEN << BOLD << "Compression successful" << RESET << "\n\n";

      std::cout << BOLD << "Compression statistics\n" << RESET;

      std::cout << "--------------------------------\n";

      std::cout << "Input:             " << inputPath << '\n';

      std::cout << "Output:            " << outputPath << '\n';

      std::cout << "Original size:     " << formatBytes(originalSize) << " ("
                << originalSize << " bytes)\n";

      std::cout << "Compressed size:   " << formatBytes(compressedSize) << " ("
                << compressedSize << " bytes)\n";

      std::cout << "LZW codes:         " << codeCount << '\n';

      if (bytesSaved >= 0) {
        std::cout << "Bytes saved:       " << GREEN << bytesSaved << RESET
                  << '\n';
      } else {
        std::cout << "Bytes saved:       " << YELLOW << bytesSaved << RESET
                  << '\n';
      }

      if (originalSize == 0) {
        std::cout << "Compression ratio: N/A\n";
        std::cout << "Size reduction:    N/A\n";
      } else {
        std::cout << std::fixed << std::setprecision(2);

        std::cout << "Compression ratio: " << compressionRatio << ":1\n";

        std::cout << "Size reduction:    ";

        if (reduction >= 0) {
          std::cout << GREEN;
        } else {
          std::cout << YELLOW;
        }

        std::cout << reduction << "%" << RESET << '\n';
      }

      std::cout << "Time:              " << std::fixed << std::setprecision(3)
                << seconds << " s\n";

      if (compressedSize > originalSize && originalSize > 0) {
        std::cout << '\n'
                  << YELLOW
                  << "Warning: compressed file is larger than the original."
                  << RESET << '\n';
      }

    } else {
      std::cout << CYAN << "Decompressing " << RESET << inputPath << "...\n";

      decompressFile(inputPath, outputPath);

      auto end = std::chrono::steady_clock::now();

      double seconds = std::chrono::duration<double>(end - start).count();

      std::uintmax_t compressedSize = std::filesystem::file_size(inputPath);

      std::uintmax_t restoredSize = std::filesystem::file_size(outputPath);

      std::cout << '\n'
                << GREEN << BOLD << "Decompression successful" << RESET
                << "\n\n";

      std::cout << BOLD << "Decompression statistics\n" << RESET;

      std::cout << "--------------------------------\n";

      std::cout << "Input:             " << inputPath << '\n';

      std::cout << "Output:            " << outputPath << '\n';

      std::cout << "Compressed size:   " << formatBytes(compressedSize) << '\n';

      std::cout << "Restored size:     " << formatBytes(restoredSize) << '\n';

      std::cout << "Time:              " << std::fixed << std::setprecision(3)
                << seconds << " s\n";
    }

  } catch (const std::exception &e) {
    std::cerr << RED << "Error: " << e.what() << '\n' << RESET;

    return 1;
  }

  return 0;
}
