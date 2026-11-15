#include "Board.h"

#include <algorithm>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>

namespace memory {

namespace {

// Every emoji here renders two terminal columns wide, so the grid stays aligned.
const std::vector<std::string> kEmoji = {
    "😀", "🐶", "🍕", "🚀", "🎸", "🌈", "🍉", "🦄", "🎲",
    "🐙", "🌵", "🍩", "🐢", "🎈", "🔥", "🍀", "👑", "🐼",
};

const std::vector<std::string> kAscii = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
};

const std::vector<std::string>& symbolsFor(SymbolSet set) {
    return set == SymbolSet::Emoji ? kEmoji : kAscii;
}

const char* const kReset = "\033[0m";
const char* const kDim = "\033[2m";
const char* const kFaceUp = "\033[1;33m";
const char* const kMatched = "\033[32m";

}  // namespace

std::size_t Board::maxPairs(SymbolSet symbols) {
    return symbolsFor(symbols).size();
}

Board::Board(int rows, int cols, unsigned seed, SymbolSet symbols)
    : rows_(rows), cols_(cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Board dimensions must be positive");
    }
    if ((rows * cols) % 2 != 0) {
        throw std::invalid_argument("Board must have an even number of cards");
    }
    const auto& pool = symbolsFor(symbols);
    const std::size_t pairs = static_cast<std::size_t>(rows * cols) / 2;
    if (pairs > pool.size()) {
        throw std::invalid_argument("Board too large: at most " +
                                    std::to_string(pool.size() * 2) + " cards");
    }

    std::mt19937 rng(seed);
    std::vector<std::string> chosen(pool.begin(), pool.end());
    std::shuffle(chosen.begin(), chosen.end(), rng);

    cards_.reserve(pairs * 2);
    for (std::size_t i = 0; i < pairs; ++i) {
        std::string symbol = chosen[i];
        if (symbols == SymbolSet::Ascii) {
            symbol = " " + symbol;  // pad to two columns, same width as emoji
        }
        cards_.emplace_back(symbol);
        cards_.emplace_back(symbol);
    }
    std::shuffle(cards_.begin(), cards_.end(), rng);
}

bool Board::isValidPosition(int pos) const noexcept {
    return pos >= 1 && pos <= size();
}

bool Board::isSelectable(int pos) const noexcept {
    if (!isValidPosition(pos)) {
        return false;
    }
    const Card& card = cards_[static_cast<std::size_t>(pos - 1)];
    return !card.isMatched() && !card.isFaceUp();
}

Card& Board::at(int pos) {
    if (!isValidPosition(pos)) {
        throw std::out_of_range("Invalid card position " + std::to_string(pos));
    }
    return cards_[static_cast<std::size_t>(pos - 1)];
}

const Card& Board::at(int pos) const {
    return const_cast<Board*>(this)->at(pos);
}

int Board::pairsRemaining() const noexcept {
    const auto unmatched = std::count_if(cards_.begin(), cards_.end(),
                                         [](const Card& c) { return !c.isMatched(); });
    return static_cast<int>(unmatched / 2);
}

void Board::render(std::ostream& out, const RenderStyle& style) const {
    // Boards hold at most 52 cards, so every cell is two columns wide.
    std::string border = "  +";
    for (int c = 0; c < cols_; ++c) {
        border += std::string(6, '-') + "+";
    }

    out << border << '\n';
    for (int r = 0; r < rows_; ++r) {
        out << "  |";
        for (int c = 0; c < cols_; ++c) {
            const int pos = r * cols_ + c + 1;
            const Card& card = cards_[static_cast<std::size_t>(pos - 1)];

            std::ostringstream cell;
            const char* colorCode = nullptr;
            if (card.isMatched()) {
                cell << card.symbol();
                colorCode = kMatched;
            } else if (card.isFaceUp()) {
                cell << card.symbol();
                colorCode = kFaceUp;
            } else {
                cell << std::setw(2) << std::setfill('0') << pos;
                colorCode = kDim;
            }

            out << "  ";
            if (style.color) out << colorCode;
            out << cell.str();
            if (style.color) out << kReset;
            out << "  |";
        }
        out << '\n' << border << '\n';
    }
}

}  // namespace memory
