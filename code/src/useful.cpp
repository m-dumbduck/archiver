#include "useful.h"

std::vector<bool> Useful::GetNext(const std::vector<bool>& value) {
    std::vector<bool> next = value;
    for (int32_t i = static_cast<int32_t>(next.size()) - 1; i >= 0; --i) {
        next[i] = next[i] ^ 1;
        if (next[i]) {
            break;
        }
    }
    return next;
}
