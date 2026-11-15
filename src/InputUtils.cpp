#include "InputUtils.h"

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>

namespace memory::input {

std::string trim(const std::string& s) {
    const auto isSpace = [](unsigned char c) { return std::isspace(c) != 0; };
    std::size_t begin = 0;
    std::size_t end = s.size();
    while (begin < end && isSpace(static_cast<unsigned char>(s[begin]))) ++begin;
    while (end > begin && isSpace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(begin, end - begin);
}

bool parseInt(const std::string& s, int& out) {
    const std::string t = trim(s);
    if (t.empty()) return false;
    errno = 0;
    char* end = nullptr;
    const long value = std::strtol(t.c_str(), &end, 10);
    if (errno != 0 || *end != '\0' || value < INT_MIN || value > INT_MAX) return false;
    out = static_cast<int>(value);
    return true;
}

bool isQuit(const std::string& s) {
    return s == "q" || s == "Q" || s == "quit" || s == "exit";
}

}  // namespace memory::input
