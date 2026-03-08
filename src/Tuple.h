#pragma once

// Пара значений произвольных типов
template <class T1, class T2>
struct Tuple {
    T1 first;
    T2 second;

    Tuple() = default;
    Tuple(T1 f, T2 s) : first(std::move(f)), second(std::move(s)) {}

    bool operator==(const Tuple& o) const {
        return first == o.first && second == o.second;
    }
};
