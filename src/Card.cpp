#include "Card.h"

#include <utility>

namespace memory {

Card::Card(std::string symbol) : symbol_(std::move(symbol)) {}

void Card::markMatched() noexcept {
    matched_ = true;
    faceUp_ = true;
}

bool Card::matches(const Card& other) const noexcept {
    return symbol_ == other.symbol_;
}

}  // namespace memory
