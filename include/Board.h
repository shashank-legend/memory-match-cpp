#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include "Card.h"

namespace memory {

enum class SymbolSet { Emoji, Ascii };

struct RenderStyle {
    bool color = true;
};

// A rows x cols grid of shuffled card pairs. Positions are 1-based,
// numbered left-to-right, top-to-bottom, exactly as shown to the player.
class Board {
public:
    Board(int rows, int cols, unsigned seed, SymbolSet symbols = SymbolSet::Emoji);

    static std::size_t maxPairs(SymbolSet symbols);

    int rows() const noexcept { return rows_; }
    int cols() const noexcept { return cols_; }
    int size() const noexcept { return rows_ * cols_; }

    bool isValidPosition(int pos) const noexcept;
    // Valid, not matched and not currently face up.
    bool isSelectable(int pos) const noexcept;

    Card& at(int pos);
    const Card& at(int pos) const;

    int pairsRemaining() const noexcept;
    bool allMatched() const noexcept { return pairsRemaining() == 0; }

    void render(std::ostream& out, const RenderStyle& style = {}) const;

private:
    int rows_;
    int cols_;
    std::vector<Card> cards_;
};

}  // namespace memory
