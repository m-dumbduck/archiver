#pragma once

#include "useful.h"

#include <cstdint>
#include <fstream>

class Writer {
public:
    Writer();

    explicit Writer(std::ostream* stream);

    void WriteBitVector(const std::vector<bool>& bits);

    void WriteReversedSymbolFromInt(uint32_t value, size_t size);

    void WriteChar(uint32_t symbol);

    void Close();

private:
    std::ostream* stream_;
    uint8_t buffer_;
    size_t buffer_size_;

    void WriteBit(bool bit);
};
