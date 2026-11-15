#include "Game.h"

#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <utility>

#include "InputUtils.h"

namespace memory {

Game::Game(Board board, std::vector<Player> players, std::istream& in, std::ostream& out,
           GameOptions options)
    : board_(std::move(board)),
      players_(std::move(players)),
      in_(in),
      out_(out),
      options_(options) {
    if (players_.empty()) {
        throw std::invalid_argument("A game needs at least one player");
    }
}

GameResult Game::run() {
    std::size_t turn = 0;

    while (!board_.allMatched()) {
        Player& player = players_[turn % players_.size()];

        redraw(player);
        const int first = promptForCard("first");
        if (first == 0) return GameResult::Quit;
        board_.at(first).flipUp();

        status_.clear();
        redraw(player);
        const int second = promptForCard("second");
        if (second == 0) {
            board_.at(first).flipDown();
            return GameResult::Quit;
        }
        board_.at(second).flipUp();

        const bool matched = board_.at(first).matches(board_.at(second));
        player.recordAttempt(matched);

        if (matched) {
            board_.at(first).markMatched();
            board_.at(second).markMatched();
            status_ = "Match! " + player.name() + " scores and goes again.";
            continue;
        }

        redraw(player);
        out_ << "No match. Press Enter to hide the cards and pass the turn...";
        out_.flush();
        const bool keepPlaying = waitForEnter();
        board_.at(first).flipDown();
        board_.at(second).flipDown();
        if (!keepPlaying) return GameResult::Quit;

        status_.clear();
        ++turn;
    }

    status_ = "All pairs found!";
    redraw(players_[turn % players_.size()]);
    printScoreboard();
    return GameResult::Finished;
}

std::vector<const Player*> Game::leaders() const {
    int best = 0;
    for (const auto& p : players_) best = std::max(best, p.score());

    std::vector<const Player*> result;
    for (const auto& p : players_) {
        if (p.score() == best) result.push_back(&p);
    }
    return result;
}

int Game::promptForCard(const std::string& label) {
    while (true) {
        out_ << "Pick the " << label << " card (1-" << board_.size() << ", q to quit): ";
        out_.flush();

        std::string line;
        if (!std::getline(in_, line)) {
            out_ << '\n';
            return 0;
        }
        line = input::trim(line);
        if (input::isQuit(line)) return 0;

        int pos = 0;
        if (!input::parseInt(line, pos) || !board_.isValidPosition(pos)) {
            out_ << "  Please enter a number between 1 and " << board_.size() << ".\n";
        } else if (board_.at(pos).isMatched()) {
            out_ << "  Card " << pos << " is already matched. Choose another.\n";
        } else if (board_.at(pos).isFaceUp()) {
            out_ << "  Card " << pos << " is already face up. Choose another.\n";
        } else {
            return pos;
        }
    }
}

bool Game::waitForEnter() {
    std::string line;
    if (!std::getline(in_, line)) return false;
    return !input::isQuit(input::trim(line));
}

void Game::redraw(const Player& current) {
    if (options_.clearScreen) {
        out_ << "\033[2J\033[H";
    }
    out_ << "\n  MEMORY MATCH  |  Pairs left: " << board_.pairsRemaining() << "\n\n";
    board_.render(out_, RenderStyle{options_.color});

    out_ << "\n  Scores: ";
    for (std::size_t i = 0; i < players_.size(); ++i) {
        if (i) out_ << "  |  ";
        out_ << players_[i].name() << ' ' << players_[i].score();
    }
    out_ << "\n";
    if (!board_.allMatched()) {
        out_ << "  Turn:   " << current.name() << "\n";
    }
    if (!status_.empty()) {
        out_ << "\n  " << status_ << "\n";
    }
    out_ << '\n';
}

void Game::printScoreboard() {
    std::vector<const Player*> ranked;
    for (const auto& p : players_) ranked.push_back(&p);
    std::stable_sort(ranked.begin(), ranked.end(),
                     [](const Player* a, const Player* b) { return a->score() > b->score(); });

    out_ << "  Final results\n";
    out_ << "  " << std::left << std::setw(16) << "Player" << std::right << std::setw(7)
         << "Pairs" << std::setw(10) << "Attempts" << std::setw(10) << "Accuracy" << '\n';
    for (const Player* p : ranked) {
        out_ << "  " << std::left << std::setw(16) << p->name() << std::right << std::setw(7)
             << p->score() << std::setw(10) << p->attempts() << std::setw(9) << std::fixed
             << std::setprecision(0) << p->accuracy() << "%\n";
    }
    out_ << '\n';

    if (players_.size() == 1) {
        out_ << "  Solved in " << players_.front().attempts() << " attempts.\n";
        return;
    }
    const auto top = leaders();
    if (top.size() == 1) {
        out_ << "  Winner: " << top.front()->name() << "!\n";
    } else {
        out_ << "  It's a tie between ";
        for (std::size_t i = 0; i < top.size(); ++i) {
            if (i) out_ << (i + 1 == top.size() ? " and " : ", ");
            out_ << top[i]->name();
        }
        out_ << "!\n";
    }
}

}  // namespace memory
