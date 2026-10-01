#ifndef FILE_IO_H
#define FILE_IO_H

#include <cstddef>
#include <string>
#include <vector>

std::string readBinaryFile(const std::string& path);

void writeBinaryFile(const std::string& path, const std::string& data);

void writeCompressedFile(const std::string& path,
                         const std::vector<int>& codes);

std::vector<int> readCompressedFile(const std::string& path);

std::size_t compressFile(const std::string& inputPath,
                         const std::string& outputPath);

void decompressFile(const std::string& inputPath,
                    const std::string& outputPath);

#endif
