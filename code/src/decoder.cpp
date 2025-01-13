#include "decoder.h"

#include <iostream>
#include <sstream>

Decoder::Decoder(const std::string& archive_name) {
    std::ifstream archive_file_stream(archive_name);
    is_ = Reader(&archive_file_stream);
    if (!is_.Good()) {
        throw std::runtime_error("file with name " + archive_name + " does not exist or cannot be read");
    }
    std::cout << "### decoding " << archive_name << std::endl;
    while (DecodeFile()) {
    }
    std::cout << "### decoding of " << archive_name << " done!" << std::endl;
}

uint32_t Decoder::ReadSymbol() {
    while (true) {
        if (is_.FileEnded()) {
            return NOT_IN_SYMBOLS;
        }
        bool bit = is_.GetBit();
        uint32_t cur = trie_.Move(bit);
        if (cur != NOT_IN_SYMBOLS) {
            return cur;
        }
    }
}

void Decoder::RestoreCanonicalCodes() {
    const std::size_t symbols_count = is_.Get9Bits();
    canonical_pairs_.resize(symbols_count);
    for (std::size_t i = 0; i < symbols_count; ++i) {
        canonical_pairs_[i].symbol = is_.Get9Bits();
    }

    std::size_t cur_symbols_count = 0;
    symbol_code_size_.resize(symbols_count, 0);
    for (std::size_t cur_size = 1; cur_symbols_count < symbols_count; ++cur_size) {
        auto cur_num = is_.Get9Bits();
        for (std::size_t i = 0; i < cur_num; ++i) {
            symbol_code_size_[cur_symbols_count + i] = cur_size;
        }
        cur_symbols_count += cur_num;
    }

    canonical_pairs_[0].code = std::vector<bool>(symbol_code_size_[0], false);
    for (std::size_t i = 1; i < canonical_pairs_.size(); ++i) {
        canonical_pairs_[i].code = Useful::GetNext(canonical_pairs_[i - 1].code);
        if (symbol_code_size_[i] > symbol_code_size_[i - 1]) {
            for (std::size_t j = 0; j + symbol_code_size_[i - 1] < symbol_code_size_[i]; ++j) {
                canonical_pairs_[i].code.push_back(false);
            }
        }
    }
}

uint32_t Decoder::WriteStream(std::ostream* stream) {
    Writer writer(stream);
    uint32_t symbol = 0;
    while (symbol != constants::ARCHIVE_END && symbol != constants::ONE_MORE_FILE &&
           symbol != constants::FILENAME_END) {
        symbol = ReadSymbol();
        if (symbol != constants::ARCHIVE_END && symbol != constants::ONE_MORE_FILE &&
            symbol != constants::FILENAME_END) {
            writer.WriteChar(symbol);
        }
    }
    return symbol;
}

bool Decoder::DecodeFile() {
    RestoreCanonicalCodes();

    trie_ = BinaryTrie(canonical_pairs_);

    std::ostringstream filename_stream;
    WriteStream(&filename_stream);

    std::ofstream current_output_file_stream(filename_stream.str());

    return WriteStream(&current_output_file_stream) == constants::ONE_MORE_FILE;
}
