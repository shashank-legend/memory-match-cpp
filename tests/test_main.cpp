// Minimal self-contained test runner (no external dependencies).
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Board.h"
#include "Card.h"
#include "Game.h"
#include "InputUtils.h"
#include "Player.h"

using namespace memory;

namespace {

int failures = 0;
std::vector<std::pair<std::string, std::function<void()>>>& registry() {
    static std::vector<std::pair<std::string, std::function<void()>>> tests;
    return tests;
}

struct Register {
    Register(const std::string& name, std::function<void()> fn) {
        registry().emplace_back(name, std::move(fn));
    }
};

#define TEST(name)                                   \
    void name();                                     \
    const Register reg_##name(#name, name);          \
    void name()

#define CHECK(cond)                                                                    \
    do {                                                                               \
        if (!(cond)) {                                                                 \
            std::cerr << "    " << __FILE__ << ':' << __LINE__ << ": CHECK(" #cond ")\n"; \
            ++failures;                                                                \
        }                                                                              \
    } while (0)

template <typename F>
bool throws(F fn) {
    try {
        fn();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}

// Groups board positions by symbol: each entry is a pair of positions.
std::vector<std::pair<int, int>> pairsOf(const Board& board) {
    std::map<std::string, std::vector<int>> bySymbol;
    for (int pos = 1; pos <= board.size(); ++pos) bySymbol[board.at(pos).symbol()].push_back(pos);
    std::vector<std::pair<int, int>> pairs;
    for (const auto& [symbol, positions] : bySymbol) pairs.emplace_back(positions[0], positions[1]);
    return pairs;
}

const GameOptions kPlain{false, false};

// ---- Card -------------------------------------------------------------------

TEST(card_matching_and_state) {
    Card a("X"), b("X"), c("Y");
    CHECK(a.matches(b));
    CHECK(!a.matches(c));
    CHECK(!a.isFaceUp());
    a.flipUp();
    CHECK(a.isFaceUp());
    a.flipDown();
    CHECK(!a.isFaceUp());
    a.markMatched();
    CHECK(a.isMatched());
    CHECK(a.isFaceUp());
}

// ---- Player -----------------------------------------------------------------

TEST(player_scoring) {
    Player p("Ada");
    CHECK(p.name() == "Ada");
    CHECK(p.accuracy() == 0.0);
    p.recordAttempt(true);
    p.recordAttempt(false);
    CHECK(p.score() == 1);
    CHECK(p.attempts() == 2);
    CHECK(p.accuracy() == 50.0);
    p.reset();
    CHECK(p.score() == 0 && p.attempts() == 0);
}

// ---- Board ------------------------------------------------------------------

TEST(board_has_exactly_two_of_each_symbol) {
    Board board(4, 4, 42);
    std::map<std::string, int> counts;
    for (int pos = 1; pos <= board.size(); ++pos) ++counts[board.at(pos).symbol()];
    CHECK(counts.size() == 8);
    for (const auto& [symbol, n] : counts) CHECK(n == 2);
    CHECK(board.pairsRemaining() == 8);
    CHECK(!board.allMatched());
}

TEST(board_same_seed_same_layout) {
    Board a(4, 4, 7), b(4, 4, 7);
    bool same = true;
    for (int pos = 1; pos <= a.size(); ++pos) same = same && a.at(pos).matches(b.at(pos));
    CHECK(same);
}

TEST(board_rejects_bad_dimensions) {
    CHECK(throws([] { Board(3, 3, 1); }));
    CHECK(throws([] { Board(0, 4, 1); }));
    CHECK(throws([] { Board(8, 8, 1); }));                    // 32 pairs > 18 emoji
    CHECK(!throws([] { Board(6, 6, 1); }));
    CHECK(!throws([] { Board(4, 13, 1, SymbolSet::Ascii); })); // 26 pairs
}

TEST(board_positions_and_selection) {
    Board board(2, 4, 1);
    CHECK(!board.isValidPosition(0));
    CHECK(board.isValidPosition(1));
    CHECK(board.isValidPosition(8));
    CHECK(!board.isValidPosition(9));
    CHECK(throws([&] { board.at(9); }));
    board.at(3).flipUp();
    CHECK(!board.isSelectable(3));
    CHECK(board.isSelectable(4));
}

TEST(board_render_hides_unflipped_cards) {
    Board board(2, 2, 3, SymbolSet::Ascii);
    std::ostringstream out;
    board.render(out, RenderStyle{false});
    const std::string hidden = out.str();
    CHECK(hidden.find("01") != std::string::npos);
    CHECK(hidden.find(board.at(1).symbol()) == std::string::npos);

    board.at(1).flipUp();
    std::ostringstream out2;
    board.render(out2, RenderStyle{false});
    CHECK(out2.str().find(board.at(1).symbol()) != std::string::npos);
}

// ---- Input ------------------------------------------------------------------

TEST(input_parsing) {
    int n = 0;
    CHECK(input::parseInt(" 12 ", n) && n == 12);
    CHECK(!input::parseInt("", n));
    CHECK(!input::parseInt("12abc", n));
    CHECK(!input::parseInt("abc", n));
    CHECK(!input::parseInt("99999999999999", n));
    CHECK(input::trim("  hi \t") == "hi");
    CHECK(input::isQuit("q") && input::isQuit("quit") && !input::isQuit("1"));
}

// ---- Game -------------------------------------------------------------------

TEST(game_single_player_perfect_run) {
    Board board(2, 4, 99, SymbolSet::Ascii);
    std::ostringstream script;
    for (auto [a, b] : pairsOf(board)) script << a << '\n' << b << '\n';

    std::istringstream in(script.str());
    std::ostringstream out;
    Game game(std::move(board), {Player("Solo")}, in, out, kPlain);

    CHECK(game.run() == GameResult::Finished);
    CHECK(game.board().allMatched());
    CHECK(game.players()[0].score() == 4);
    CHECK(game.players()[0].attempts() == 4);
    CHECK(out.str().find("Solved in 4 attempts") != std::string::npos);
}

TEST(game_miss_passes_turn_and_match_keeps_it) {
    Board board(2, 4, 5, SymbolSet::Ascii);
    const auto pairs = pairsOf(board);

    std::ostringstream script;
    // Alice misses (mixes two different pairs), then presses Enter.
    script << pairs[0].first << '\n' << pairs[1].first << "\n\n";
    // Bob finds every pair in a row, keeping the turn each time.
    for (auto [a, b] : pairs) script << a << '\n' << b << '\n';

    std::istringstream in(script.str());
    std::ostringstream out;
    Game game(std::move(board), {Player("Alice"), Player("Bob")}, in, out, kPlain);

    CHECK(game.run() == GameResult::Finished);
    CHECK(game.players()[0].score() == 0);
    CHECK(game.players()[0].attempts() == 1);
    CHECK(game.players()[1].score() == 4);
    CHECK(game.leaders().size() == 1 && game.leaders()[0]->name() == "Bob");
    CHECK(out.str().find("Winner: Bob!") != std::string::npos);
}

TEST(game_rejects_invalid_and_repeated_picks) {
    Board board(2, 4, 11, SymbolSet::Ascii);
    const auto pairs = pairsOf(board);

    std::ostringstream script;
    script << "abc\n0\n9\n" << pairs[0].first << '\n'
           << pairs[0].first << '\n' << pairs[0].second << '\n'   // same card twice
           << pairs[0].first << '\n'                              // already matched
           << "q\n";

    std::istringstream in(script.str());
    std::ostringstream out;
    Game game(std::move(board), {Player("Tester")}, in, out, kPlain);

    CHECK(game.run() == GameResult::Quit);
    CHECK(game.players()[0].score() == 1);
    const std::string text = out.str();
    CHECK(text.find("between 1 and 8") != std::string::npos);
    CHECK(text.find("already face up") != std::string::npos);
    CHECK(text.find("already matched") != std::string::npos);
}

TEST(game_ends_cleanly_on_eof) {
    std::istringstream in("1\n");
    std::ostringstream out;
    Game game(Board(2, 2, 1, SymbolSet::Ascii), {Player("X")}, in, out, kPlain);
    CHECK(game.run() == GameResult::Quit);
    CHECK(!game.board().at(1).isFaceUp());
}

TEST(game_requires_players) {
    std::istringstream in;
    std::ostringstream out;
    CHECK(throws([&] { Game(Board(2, 2, 1), {}, in, out); }));
}

}  // namespace

int main() {
    int failedTests = 0;
    for (const auto& [name, fn] : registry()) {
        const int before = failures;
        try {
            fn();
        } catch (const std::exception& e) {
            std::cerr << "    unexpected exception: " << e.what() << '\n';
            ++failures;
        }
        const bool ok = failures == before;
        if (!ok) ++failedTests;
        std::cout << (ok ? "[PASS] " : "[FAIL] ") << name << '\n';
    }
    std::cout << '\n' << registry().size() - failedTests << '/' << registry().size()
              << " tests passed\n";
    return failedTests == 0 ? 0 : 1;
}
