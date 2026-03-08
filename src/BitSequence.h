#pragma once
#include "ArraySequence.h"

// Бит — единица хранения в BitSequence
struct Bit {
    int value; // только 0 или 1
    Bit() : value(0) {}
    explicit Bit(int v) : value(v & 1) {}
    explicit operator int() const { return value; }
    bool operator==(const Bit& o) const { return value == o.value; }
    bool operator!=(const Bit& o) const { return value != o.value; }
};

// BitSequence — последовательность битов с логическими операциями
class BitSequence : public MutableArraySequence<Bit> {
    // Возвращает наименьшую из двух длин
    static int minLen(int a, int b) { return a < b ? a : b; }

public:
    BitSequence() = default;

    explicit BitSequence(int size) {
        for (int i = 0; i < size; ++i) AppendInPlace(Bit(0));
    }

    BitSequence(const int* bits, int count) {
        for (int i = 0; i < count; ++i) AppendInPlace(Bit(bits[i]));
    }

    BitSequence(const BitSequence&) = default;

    // побитовые операции
    BitSequence* AND(const BitSequence* other) const {
        int len = minLen(GetLength(), other->GetLength());
        auto* result = new BitSequence();
        for (int i = 0; i < len; ++i)
            result->AppendInPlace(Bit(Get(i).value & other->Get(i).value));
        return result;
    }

    BitSequence* OR(const BitSequence* other) const {
        int len = minLen(GetLength(), other->GetLength());
        auto* result = new BitSequence();
        for (int i = 0; i < len; ++i)
            result->AppendInPlace(Bit(Get(i).value | other->Get(i).value));
        return result;
    }

    BitSequence* XOR(const BitSequence* other) const {
        int len = minLen(GetLength(), other->GetLength());
        auto* result = new BitSequence();
        for (int i = 0; i < len; ++i)
            result->AppendInPlace(Bit(Get(i).value ^ other->Get(i).value));
        return result;
    }

    BitSequence* NOT() const {
        auto* result = new BitSequence();
        for (int i = 0; i < GetLength(); ++i)
            result->AppendInPlace(Bit(1 - Get(i).value));
        return result;
    }

    // применение маски
    BitSequence* ApplyMask(const BitSequence* mask) const { return AND(mask); }

    // вывод в строку
    std::string ToString() const {
        std::string s;
        for (int i = 0; i < GetLength(); ++i) s += static_cast<char>('0' + Get(i).value);
        return s;
    }
};
