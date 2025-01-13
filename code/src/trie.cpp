#include "trie.h"

BinaryTrie::BinaryTrie() : root_(nullptr), cur_(nullptr) {
}

BinaryTrie::BinaryTrie(const BinaryTrie& other) {
    *this = other;
}

BinaryTrie::BinaryTrie(uint32_t symbol) {
    root_ = std::make_shared<Node>(nullptr, nullptr, symbol);
    cur_ = root_;
}

BinaryTrie::BinaryTrie(const std::vector<SymbolCode>& encoding) {
    root_ = std::make_shared<Node>(Node(nullptr, nullptr, NOT_IN_SYMBOLS));
    cur_ = root_;
    for (const auto& [symbol, code] : encoding) {
        std::shared_ptr<Node> current = root_;
        for (auto bit : code) {
            if (!bit) {
                if (!current->left_) {
                    current->left_ = std::make_shared<Node>(Node(nullptr, nullptr, symbol));
                }
                current = current->left_;
            } else {
                if (!current->right_) {
                    current->right_ = std::make_shared<Node>(Node(nullptr, nullptr, symbol));
                }
                current = current->right_;
            }
        }
    }
}

BinaryTrie::BinaryTrie(const BinaryTrie& left_trie, const BinaryTrie& right_trie) {
    uint32_t symbol = NOT_IN_SYMBOLS;
    if (left_trie.root_) {
        symbol = left_trie.root_->symbol_;
    }
    if (right_trie.root_) {
        symbol = std::min(right_trie.root_->symbol_, symbol);
    }
    root_ = std::make_shared<Node>(left_trie.root_, right_trie.root_, symbol);
    cur_ = root_;
}

std::vector<SymbolCode> BinaryTrie::GetAllCodes() const {
    std::vector<SymbolCode> result;
    std::vector<bool> cur;
    Dfs(root_, cur, result);
    return result;
}

void BinaryTrie::Dfs(const std::shared_ptr<Node> v, std::vector<bool>& cur, std::vector<SymbolCode>& result) {
    if (v->left_ == nullptr && v->right_ == nullptr) {
        result.push_back(SymbolCode(v->symbol_, cur));
        return;
    }
    cur.push_back(false);
    Dfs(v->left_, cur, result);
    cur.back() = true;
    Dfs(v->right_, cur, result);
    cur.pop_back();
}

uint32_t BinaryTrie::Move(bool bit) {
    if (!bit) {
        cur_ = cur_->left_;
    } else {
        cur_ = cur_->right_;
    }
    if (!cur_->left_ || !cur_->right_) {
        uint32_t result = cur_->symbol_;
        cur_ = root_;
        return result;
    }
    return NOT_IN_SYMBOLS;
}

bool BinaryTrie::operator<(const BinaryTrie& other) const {
    return root_->symbol_ < other.root_->symbol_;
}

BinaryTrie& BinaryTrie::operator=(const BinaryTrie& other) {
    root_ = other.root_;
    cur_ = other.cur_;
    return *this;
}
