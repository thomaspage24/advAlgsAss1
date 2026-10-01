# LZW Compressor

A command-line file compression tool implementing the Lempel-Ziv-Welch (LZW) compression algorithm in C++.

The program supports compression and decompression of binary files using variable-width LZW codes.

## Build

Compile the program with:

```bash
g++ -std=c++20 \
src/main.cpp \
src/lzw.cpp \
src/file_io.cpp \
src/bit_io.cpp \
-o lzw
```

## Compress

```bash
./lzw compress <input> <output>
```

Example:

```bash
./lzw compress samples/normal.txt samples/normal.lzw
```

The program displays:

- Original file size
- Compressed file size
- Number of LZW codes
- Bytes saved
- Compression ratio
- Percentage size reduction

## Decompress

```bash
./lzw decompress <input.lzw> <output>
```

Example:

```bash
./lzw decompress samples/normal.lzw samples/restored.txt
```

The decompressed file can be checked against the original using:

```bash
cmp samples/normal.txt samples/restored.txt
```

No output from `cmp` means the files are identical.

## File Format

Compressed files use the custom `LZW2` format.

The file begins with the header:

```text
LZW2
```

LZW codes are then stored using variable-width bit packing.

Code widths begin at 9 bits and increase as the dictionary grows:

```text
0-511       -> 9 bits
512-1023    -> 10 bits
1024-2047   -> 11 bits
...
```

The maximum supported LZW code is `65535`.

## Tests

The project uses GoogleTest.

Test files:

```text
tests/test_lzw.cpp
tests/test_bit_io.cpp
tests/test_file_io.cpp
```

Compile all tests with:

```bash
g++ -std=c++20 \
tests/test_lzw.cpp \
tests/test_bit_io.cpp \
tests/test_file_io.cpp \
src/lzw.cpp \
src/bit_io.cpp \
src/file_io.cpp \
-I$(brew --prefix googletest)/include \
-L$(brew --prefix googletest)/lib \
-lgtest \
-lgtest_main \
-pthread \
-o tests_run
```

Run:

```bash
./tests_run
```

## Project Structure

```text
lzw-compress/
├── src/
│   ├── main.cpp
│   ├── lzw.cpp
│   ├── lzw.h
│   ├── file_io.cpp
│   ├── file_io.h
│   ├── bit_io.cpp
│   └── bit_io.h
├── tests/
│   ├── test_lzw.cpp
│   ├── test_bit_io.cpp
│   └── test_file_io.cpp
├── samples/
└── README.md
```
