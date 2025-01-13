#include "encoder.h"
#include "useful.h"
#include "heap.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>

Encoder::Encoder(const std::vector<std::string>& filenames, const std::string& archive_name)
    : archive_name_(archive_name), codes_(constants::CNT_SIZE) {
    std::ofstream archive_stream(archive_name);
    os_ = Writer(&archive_stream);
    for (std::size_t i = 0; i < filenames.size(); ++i) {
        filename_ = filenames[i];
        EncodeFile(i + 1 == filenames.size());
    }
    os_.Close();
    archive_stream.close();
}

void Encoder::BuildTrie() {
    auto cnt = CalculateFrequencies();
    Heap<std::pair<int64_t, BinaryTrie>> q;
    for (std::size_t i = 0; i < cnt.size(); ++i) {
        if (cnt[i] > 0) {
            q.Push({cnt[i], BinaryTrie(i)});
        }
    }

    while (q.Size() > 1) {
        auto [cnt1, binary_trie1] = q.Top();
        q.Pop();
        auto [cnt2, binary_trie2] = q.Top();
        q.Pop();
        q.Push({cnt1 + cnt2, BinaryTrie(binary_trie1, binary_trie2)});
    }
    trie_ = q.Top().second;
}

void Encoder::EncodeFile(bool last_file) {
    std::ifstream current_file_stream(filename_);
    is_ = Reader(&current_file_stream);
    if (!is_.Good()) {
        os_.Close();
        std::remove(archive_name_.c_str());
        throw std::runtime_error("file with name " + filename_ + " does not exist or cannot be read");
    }

    std::cout << "### encoding file: " << filename_ << std::endl;

    filename_ = std::filesystem::path(filename_).filename();
    BuildTrie();
    canonical_pairs_ = trie_.GetAllCodes();
    MakeCanonical();

    size_t symbols_count = canonical_pairs_.size();
    os_.WriteReversedSymbolFromInt(symbols_count, constants::BITS_9);
    std::size_t max_code_size = 0;
    for (const auto& [symbol, code] : canonical_pairs_) {
        os_.WriteReversedSymbolFromInt(symbol, constants::BITS_9);
        max_code_size = std::max(max_code_size, code.size());
    }

    std::vector<size_t> code_sizes(max_code_size);
    for (const auto& [symbol, code] : canonical_pairs_) {
        ++code_sizes[code.size() - 1];
    }
    for (const auto value : code_sizes) {
        os_.WriteReversedSymbolFromInt(value, constants::BITS_9);
    }

    for (const auto& [symbol, code] : canonical_pairs_) {
        codes_[symbol] = code;
    }

    std::stringstream filename_stream(filename_);
    WriteStream(&filename_stream);
    os_.WriteBitVector(codes_[constants::FILENAME_END]);

    is_.JumpToBeginning();
    WriteStream(&current_file_stream);

    if (last_file) {
        os_.WriteBitVector(codes_[constants::ARCHIVE_END]);
    } else {
        os_.WriteBitVector(codes_[constants::ONE_MORE_FILE]);
    }
    std::cout << "### encoding of " << filename_ << " done!" << std::endl;
}

void Encoder::WriteStream(std::istream* stream) {
    Reader reader(stream);
    while (true) {
        auto to_write = codes_[reader.GetChar()];
        if (reader.FileEnded()) {
            break;
        }
        os_.WriteBitVector(to_write);
    }
}

std::vector<int64_t> Encoder::CalculateFrequencies() {
    std::vector<int64_t> cnt(constants::CNT_SIZE);
    std::fill(cnt.begin(), cnt.end(), 0);
    cnt[constants::FILENAME_END] = 1;
    cnt[constants::ONE_MORE_FILE] = 1;
    cnt[constants::ARCHIVE_END] = 1;

    for (auto symbol : filename_) {
        ++cnt[symbol];
    }
    while (true) {
        auto symbol = is_.GetChar();
        if (is_.FileEnded()) {
            break;
        }
        ++cnt[symbol];
    }
    return cnt;
}

void Encoder::MakeCanonical() {
    std::sort(canonical_pairs_.begin(), canonical_pairs_.end(), [](auto& a, auto& b) {
        if (a.code.size() != b.code.size()) {
            return a.code.size() < b.code.size();
        }
        return a.symbol < b.symbol;
    });

    canonical_pairs_[0].code = std::vector<bool>(canonical_pairs_[0].code.size(), false);
    for (int32_t i = 1; i < canonical_pairs_.size(); ++i) {
        std::size_t cur_sz = canonical_pairs_[i].code.size();
        canonical_pairs_[i].code = Useful::GetNext(canonical_pairs_[i - 1].code);
        if (cur_sz > canonical_pairs_[i].code.size()) {
            while (canonical_pairs_[i].code.size() < cur_sz) {
                canonical_pairs_[i].code.push_back(false);
            }
        }
    }
}
