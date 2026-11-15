#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "Board.h"
#include "Player.h"

namespace memory {

struct GameOptions {
    bool clearScreen = true;
    bool color = true;
};

enum class GameResult { Finished, Quit };

// Runs a turn-based memory match between one or more players.
// A player who finds a pair keeps the turn; a miss passes it on.
class Game {
public:
    Game(Board board, std::vector<Player> players, std::istream& in, std::ostream& out,
         GameOptions options = {});

    GameResult run();

    const Board& board() const noexcept { return board_; }
    const std::vector<Player>& players() const noexcept { return players_; }

    // Players with the highest score (more than one on a tie).
    std::vector<const Player*> leaders() const;

private:
    // Returns the chosen 1-based position, or 0 if the player quit / input ended.
    int promptForCard(const std::string& label);
    bool waitForEnter();
    void redraw(const Player& current);
    void printScoreboard();

    Board board_;
    std::vector<Player> players_;
    std::istream& in_;
    std::ostream& out_;
    GameOptions options_;
    std::string status_;
};

}  // namespace memory
