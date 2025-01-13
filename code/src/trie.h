#pragma once

#include <cstdint>
#include <memory>
#include <vector>

static const uint32_t NOT_IN_SYMBOLS = 1'000'000;

struct SymbolCode {
    uint32_t symbol;
    std::vector<bool> code;
    SymbolCode() : symbol(NOT_IN_SYMBOLS) {
    }
    SymbolCode(uint32_t symbol, const std::vector<bool>& code) : symbol(symbol), code(code) {
    }
};

class BinaryTrie {
public:
    BinaryTrie();

    explicit BinaryTrie(uint32_t symbol);

    explicit BinaryTrie(const std::vector<SymbolCode>& encoding);

    BinaryTrie(const BinaryTrie& other);

    BinaryTrie(const BinaryTrie& left_trie, const BinaryTrie& right_trie);

    std::vector<SymbolCode> GetAllCodes() const;

    uint32_t Move(bool bit);

    BinaryTrie& operator=(const BinaryTrie& other);

    bool operator<(const BinaryTrie& other) const;

private:
    class Node {
    public:
        Node() : left_(nullptr), right_(nullptr), symbol_(NOT_IN_SYMBOLS) {
        }
        Node(std::shared_ptr<Node> l, std::shared_ptr<Node> r, uint32_t min_char)
            : left_(l), right_(r), symbol_(min_char) {
        }
        explicit Node(std::shared_ptr<Node> other) {
            if (!other) {
                left_ = nullptr;
                right_ = nullptr;
                symbol_ = NOT_IN_SYMBOLS;
                return;
            }
            symbol_ = other->symbol_;
            left_ = other->left_;
            right_ = other->right_;
        }

    private:
        std::shared_ptr<Node> left_;
        std::shared_ptr<Node> right_;
        uint32_t symbol_;

        friend class BinaryTrie;
    };

    std::shared_ptr<Node> root_;
    std::shared_ptr<Node> cur_;

    static void Dfs(const std::shared_ptr<Node> v, std::vector<bool>& cur, std::vector<SymbolCode>& result);
};
