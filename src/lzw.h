#ifndef LZW_H
#define LZW_H

#include <cstddef>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

std::vector<int> compress(const std::string& input);

std::string decompress(const std::vector<int>& codes);

std::size_t compressStream(std::istream& input, std::ostream& output);

void decompressStream(std::istream& input, std::ostream& output);

#endif
