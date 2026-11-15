#include "Player.h"

#include <utility>

namespace memory {

Player::Player(std::string name) : name_(std::move(name)) {}

double Player::accuracy() const noexcept {
    return attempts_ == 0 ? 0.0 : 100.0 * score_ / attempts_;
}

void Player::recordAttempt(bool matched) noexcept {
    ++attempts_;
    if (matched) {
        ++score_;
    }
}

void Player::reset() noexcept {
    score_ = 0;
    attempts_ = 0;
}

}  // namespace memory
