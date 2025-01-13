#pragma once

#include "useful.h"
#include "trie.h"
#include "reader.h"
#include "writer.h"

#include <string>
#include <vector>

class Encoder {
public:
    Encoder(const std::vector<std::string>& filenames, const std::string& archive_name);

private:
    std::string archive_name_;
    std::string filename_;
    Writer os_;
    Reader is_;
    BinaryTrie trie_;
    std::vector<SymbolCode> canonical_pairs_;
    std::vector<std::vector<bool>> codes_;

    void EncodeFile(bool last_file);

    void WriteStream(std::istream* stream);

    void BuildTrie();

    std::vector<int64_t> CalculateFrequencies();

    void MakeCanonical();
};
