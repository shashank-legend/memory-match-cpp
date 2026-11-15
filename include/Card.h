#pragma once

#include <string>

namespace memory {

// A single card on the board. Two cards with the same symbol form a pair.
class Card {
public:
    explicit Card(std::string symbol);

    const std::string& symbol() const noexcept { return symbol_; }
    bool isFaceUp() const noexcept { return faceUp_; }
    bool isMatched() const noexcept { return matched_; }

    void flipUp() noexcept { faceUp_ = true; }
    void flipDown() noexcept { faceUp_ = false; }
    void markMatched() noexcept;

    bool matches(const Card& other) const noexcept;

private:
    std::string symbol_;
    bool faceUp_ = false;
    bool matched_ = false;
};

}  // namespace memory
