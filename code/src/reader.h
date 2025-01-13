#pragma once

#include <cstdint>
#include <fstream>

class Reader {
public:
    Reader();

    explicit Reader(std::istream* stream);

    bool FileEnded() const;

    void JumpToBeginning();

    bool Good() const;

    uint32_t GetBits(size_t amount, bool reverse);

    uint32_t Get9Bits();

    bool GetBit();

    uint32_t GetChar();

private:
    std::istream* stream_;
    uint8_t buffer_;
    size_t buffer_size_;

    uint32_t GetCharFromStream() const;
};
