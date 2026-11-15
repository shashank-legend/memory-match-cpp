#pragma once

#include <string>

namespace memory {

class Player {
public:
    explicit Player(std::string name);

    const std::string& name() const noexcept { return name_; }
    int score() const noexcept { return score_; }
    int attempts() const noexcept { return attempts_; }

    // Percentage of attempts that found a pair (0 when no attempts yet).
    double accuracy() const noexcept;

    void recordAttempt(bool matched) noexcept;
    void reset() noexcept;

private:
    std::string name_;
    int score_ = 0;
    int attempts_ = 0;
};

}  // namespace memory
