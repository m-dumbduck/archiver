#pragma once

#include "reader.h"
#include "trie.h"
#include "writer.h"

#include <cstdint>

class Decoder {
public:
    explicit Decoder(const std::string& archive_name);

private:
    Reader is_;
    BinaryTrie trie_;
    std::vector<SymbolCode> canonical_pairs_;
    std::vector<std::size_t> symbol_code_size_;

    uint32_t ReadSymbol();

    bool DecodeFile();

    void RestoreCanonicalCodes();

    uint32_t WriteStream(std::ostream* stream);
};
