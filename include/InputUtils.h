#pragma once

#include <string>

namespace memory::input {

std::string trim(const std::string& s);

// Parses a whole string as a base-10 int. Rejects empty strings and trailing junk.
bool parseInt(const std::string& s, int& out);

bool isQuit(const std::string& s);

}  // namespace memory::input
