#include "writer.h"

Writer::Writer() : stream_(nullptr), buffer_(0), buffer_size_(0) {
}

Writer::Writer(std::ostream* stream) : stream_(stream), buffer_(0), buffer_size_(0) {
}

void Writer::WriteBitVector(const std::vector<bool>& bits) {
    for (size_t i = 0; i < bits.size(); ++i) {
        WriteBit(bits[i]);
    }
}

void Writer::WriteReversedSymbolFromInt(const uint32_t value, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        WriteBit((value >> i) & 1);
    }
}

void Writer::WriteChar(uint32_t symbol) {
    stream_->write(reinterpret_cast<char*>(&symbol), constants::CHAR_SIZE_IN_CHARS);
}

void Writer::Close() {
    if (buffer_size_ > 0) {
        WriteChar(buffer_);
    }
    stream_->flush();
    stream_ = nullptr;
}

void Writer::WriteBit(bool bit) {
    buffer_ |= bit << (constants::BUFFER_8_BITS - buffer_size_ - 1);
    ++buffer_size_;
    if (buffer_size_ == constants::BUFFER_8_BITS) {
        WriteChar(buffer_);
        buffer_ = 0;
        buffer_size_ = 0;
    }
}
