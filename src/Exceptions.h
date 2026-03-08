#pragma once
#include <stdexcept>
#include <string>

// Исключение выхода за границы индекса
class IndexOutOfRange : public std::out_of_range {
public:
    IndexOutOfRange(int idx, int maxIdx)
        : std::out_of_range(
              "IndexOutOfRange: got " + std::to_string(idx) +
              ", valid range [0, " + std::to_string(maxIdx) + "]") {}

    explicit IndexOutOfRange(const std::string& msg)
        : std::out_of_range(msg) {}
};
