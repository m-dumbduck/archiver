#pragma once

#include <functional>
#include <iostream>
#include <ostream>
#include <vector>

template <typename T, class Comparator = std::less<T>>
class Heap {
public:
    Heap() : size_(0), comparator_() {
        heap_ = {T()};
    };

    size_t Size() const {
        return size_;
    }

    T Top() const {
        return heap_[1];
    }

    void Push(const T& elem) {
        ++size_;
        heap_.push_back(elem);
        SiftUp(size_ - 1);
    }

    void Pop() {
        std::swap(heap_[1], heap_.back());
        --size_;
        heap_.pop_back();
        SiftDown(1);
    }

private:
    size_t size_;
    std::vector<T> heap_;
    Comparator comparator_;

    void SiftUp(size_t v) {
        while (v > 1 && comparator_(heap_[v], heap_[v / 2])) {
            std::swap(heap_[v / 2], heap_[v]);
            v /= 2;
        }
    }

    void SiftDown(size_t v) {
        while (2 * v <= size_) {
            size_t to = 2 * v;
            if (2 * v + 1 <= size_ && comparator_(heap_[2 * v + 1], heap_[2 * v])) {
                to = 2 * v + 1;
            }
            if (comparator_(heap_[v], heap_[to])) {
                break;
            }
            std::swap(heap_[v], heap_[to]);
            v = to;
        }
    }
};
