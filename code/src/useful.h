#pragma once

#include <cstdint>
#include <vector>

namespace constants {
static const int32_t FAILED_EXIT_CODE = 111;
static const uint32_t FILENAME_END = 256;
static const uint32_t ONE_MORE_FILE = 257;
static const uint32_t ARCHIVE_END = 258;
static const size_t CNT_SIZE = 259;

static const uint32_t BITS_1 = 1;
static const uint32_t BITS_9 = 9;

static const uint32_t BUFFER_8_BITS = 8;
static const uint32_t BUFFER_8_BITS_CUT = 255;
static const std::size_t CHAR_SIZE_IN_CHARS = 1;
static const std::size_t CHAR_SIZE_IN_BITS = 8;
}  // namespace constants

class Useful {
public:
    static std::vector<bool> GetNext(const std::vector<bool>& value);
};
