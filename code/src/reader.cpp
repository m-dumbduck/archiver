#include "reader.h"
#include "useful.h"

Reader::Reader() : stream_(nullptr), buffer_(0), buffer_size_(0) {
}

Reader::Reader(std::istream* stream) : stream_(stream), buffer_(0), buffer_size_(0) {
}

uint32_t Reader::GetCharFromStream() const {
    if (stream_->eof()) {
        return 0;
    }
    return static_cast<uint8_t>(stream_->get());
}

bool Reader::FileEnded() const {
    return stream_->eof();
}

bool Reader::Good() const {
    return stream_->good();
}

void Reader::JumpToBeginning() {
    buffer_ = 0;
    buffer_size_ = 0;
    stream_->clear();
    stream_->seekg(0);
}

bool Reader::GetBit() {
    if (buffer_size_ == 0) {
        buffer_ = static_cast<uint8_t>(GetCharFromStream());
        buffer_size_ = constants::BUFFER_8_BITS;
    }
    bool result = (buffer_ >> (buffer_size_ - 1)) & 1;
    --buffer_size_;
    return result;
}

uint32_t Reader::GetBits(size_t amount, bool reverse) {
    if (amount == 0) {
        return 0;
    }
    uint32_t result = 0;
    for (size_t i = amount - 1; i + 1 > 0; --i) {
        result |= GetBit() << i;
    }

    if (!reverse) {
        return result;
    }
    uint32_t reversed_result = 0;
    for (std::size_t i = 0; i < amount; ++i) {
        reversed_result |= ((result >> i) & 1) << (amount - i - 1);
    }
    return reversed_result;
}

uint32_t Reader::Get9Bits() {
    return GetBits(constants::BITS_9, true);
}

uint32_t Reader::GetChar() {
    return GetBits(constants::CHAR_SIZE_IN_BITS, false);
}
