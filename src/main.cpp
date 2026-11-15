#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "Board.h"
#include "Game.h"
#include "InputUtils.h"
#include "Player.h"

using namespace memory;

namespace {

constexpr int kMaxPlayers = 8;

struct Config {
    int rows = 4;
    int cols = 4;
    std::optional<unsigned> seed;
    SymbolSet symbols = SymbolSet::Emoji;
    GameOptions game;
    std::vector<std::string> playerNames;
};

void printUsage(const char* prog) {
    std::cout
        << "Usage: " << prog << " [options]\n\n"
        << "A terminal memory-match card game for 1-" << kMaxPlayers << " players.\n\n"
        << "Options:\n"
        << "  -l, --level <easy|medium|hard>  Board size: 2x4, 4x4 (default) or 6x6\n"
        << "  -r, --rows <n>                  Custom number of rows\n"
        << "  -c, --cols <n>                  Custom number of columns\n"
        << "  -p, --player <name>             Add a player (repeatable; skips the prompt)\n"
        << "  -s, --seed <n>                  Shuffle seed, for a reproducible board\n"
        << "      --ascii                     Use letters instead of emoji\n"
        << "      --no-color                  Disable ANSI colors\n"
        << "      --no-clear                  Don't clear the screen between turns\n"
        << "  -h, --help                      Show this help\n\n"
        << "During play, type a card number to flip it, or q to quit.\n";
}

int requireInt(const std::string& flag, const std::string& value) {
    int n = 0;
    if (!input::parseInt(value, n) || n <= 0) {
        throw std::invalid_argument(flag + " expects a positive number, got '" + value + "'");
    }
    return n;
}

// Returns nullopt when the program should exit immediately (e.g. --help).
std::optional<Config> parseArgs(int argc, char* argv[]) {
    Config cfg;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        const auto next = [&]() -> std::string {
            if (i + 1 >= argc) throw std::invalid_argument(arg + " requires a value");
            return argv[++i];
        };

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return std::nullopt;
        } else if (arg == "-l" || arg == "--level") {
            const std::string level = next();
            if (level == "easy") {
                cfg.rows = 2; cfg.cols = 4;
            } else if (level == "medium") {
                cfg.rows = 4; cfg.cols = 4;
            } else if (level == "hard") {
                cfg.rows = 6; cfg.cols = 6;
            } else {
                throw std::invalid_argument("Unknown level '" + level + "'");
            }
        } else if (arg == "-r" || arg == "--rows") {
            cfg.rows = requireInt(arg, next());
        } else if (arg == "-c" || arg == "--cols") {
            cfg.cols = requireInt(arg, next());
        } else if (arg == "-p" || arg == "--player") {
            cfg.playerNames.push_back(next());
        } else if (arg == "-s" || arg == "--seed") {
            cfg.seed = static_cast<unsigned>(requireInt(arg, next()));
        } else if (arg == "--ascii") {
            cfg.symbols = SymbolSet::Ascii;
        } else if (arg == "--no-color") {
            cfg.game.color = false;
        } else if (arg == "--no-clear") {
            cfg.game.clearScreen = false;
        } else {
            throw std::invalid_argument("Unknown option '" + arg + "' (try --help)");
        }
    }
    if (cfg.playerNames.size() > static_cast<std::size_t>(kMaxPlayers)) {
        throw std::invalid_argument("At most " + std::to_string(kMaxPlayers) + " players");
    }
    return cfg;
}

// Reads a line; throws if input ends so the caller can exit cleanly.
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::cout.flush();
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Input ended");
    return input::trim(line);
}

std::vector<Player> promptForPlayers() {
    int count = 0;
    while (true) {
        const std::string line = readLine("Number of players (1-" + std::to_string(kMaxPlayers) + "): ");
        if (input::parseInt(line, count) && count >= 1 && count <= kMaxPlayers) break;
        std::cout << "  Please enter a number between 1 and " << kMaxPlayers << ".\n";
    }

    std::vector<Player> players;
    for (int i = 1; i <= count; ++i) {
        std::string name = readLine("Name of player " + std::to_string(i) + ": ");
        if (name.empty()) name = "Player " + std::to_string(i);
        players.emplace_back(name);
    }
    return players;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const auto cfg = parseArgs(argc, argv);
        if (!cfg) return 0;

        const unsigned seed = cfg->seed.value_or(static_cast<unsigned>(
            std::chrono::steady_clock::now().time_since_epoch().count() ^ std::random_device{}()));
        Board board(cfg->rows, cfg->cols, seed, cfg->symbols);

        std::vector<Player> players;
        if (cfg->playerNames.empty()) {
            players = promptForPlayers();
        } else {
            for (const auto& name : cfg->playerNames) players.emplace_back(name);
        }

        Game game(std::move(board), std::move(players), std::cin, std::cout, cfg->game);
        if (game.run() == GameResult::Quit) {
            std::cout << "Game ended early. Thanks for playing!\n";
        }
        return 0;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "\nGoodbye (" << e.what() << ").\n";
        return 1;
    }
}
