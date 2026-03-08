#pragma once
#include <ostream>
#include "IEnumerable.h"
#include "Option.h"
#include "Tuple.h"
#include "Exceptions.h"

// Опережающее объявление — реализация Map/Where/... идёт после #include ArraySequence
template <class T> class MutableArraySequence;

// Абстрактный базовый класс последовательности
template <class T>
class Sequence : public IEnumerable<T> {
public:
    virtual ~Sequence() = default;

    // чтение
    virtual T   GetFirst()  const = 0;
    virtual T   GetLast()   const = 0;
    virtual T   Get(int index) const = 0;
    virtual int GetLength() const = 0;

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    // мутация
    virtual Sequence<T>* Append(T item)              = 0;
    virtual Sequence<T>* Prepend(T item)             = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* other)  = 0;

    // паттерн mutable/immutable
    virtual Sequence<T>* Clone()       const = 0;
    virtual Sequence<T>* Instance()          = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;

    // try-семантика
    Option<T> TryGetFirst() const {
        try { return Option<T>::Some(GetFirst()); } catch (...) { return Option<T>::None(); }
    }
    Option<T> TryGetLast() const {
        try { return Option<T>::Some(GetLast());  } catch (...) { return Option<T>::None(); }
    }
    Option<T> TryGet(int index) const {
        try { return Option<T>::Some(Get(index)); } catch (...) { return Option<T>::None(); }
    }

    // reduce
    template <class T2>
    T2 Reduce(T2 (*f)(T2, T), T2 initial) const {
        T2 acc = initial;
        for (int i = 0; i < GetLength(); ++i) acc = f(acc, Get(i));
        return acc;
    }

    // Find
    Option<T> Find(bool (*predicate)(T)) const {
        for (int i = 0; i < GetLength(); ++i)
            if (predicate(Get(i))) return Option<T>::Some(Get(i));
        return Option<T>::None();
    }
    Option<T> TryFind(bool (*predicate)(T)) const { return Find(predicate); }

    // операторы
    // operator[] убран из базового класса — он конфликтовал с перегрузками в производных классах (T& vs T).
    // Get(i) или operator[] подкласса.
    Sequence<T>* operator+(Sequence<T>* other) { return Concat(other); }

    bool operator==(const Sequence<T>& other) const {
        if (GetLength() != other.GetLength()) return false;
        for (int i = 0; i < GetLength(); ++i)
            if (Get(i) != other.Get(i)) return false;
        return true;
    }

    bool IsEmpty() const { return GetLength() == 0; }

    // объявления методов, реализованных после #include ArraySequence
    template <class T2> Sequence<T2>* Map(T2 (*f)(T)) const;
    template <class T2> Sequence<T2>* MapIndexed(T2 (*f)(T, int)) const;
    Sequence<T>* Where(bool (*predicate)(T)) const;
    Sequence<T>* FlatMap(Sequence<T>* (*f)(T)) const;
    Sequence<T>* Slice(int i, int n, Sequence<T>* s = nullptr) const;
    Sequence<Sequence<T>*>* Split(bool (*isSeparator)(T)) const;
};

// вывод в поток
template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
    os << "[";
    for (int i = 0; i < seq.GetLength(); ++i) {
        if (i > 0) os << ", ";
        os << seq.Get(i);
    }
    return os << "]";
}

// Разрыв цикла: включаем ArraySequence ПОСЛЕ определения Sequence
#include "ArraySequence.h"

// Реализации методов map-reduce

template <class T>
template <class T2>
Sequence<T2>* Sequence<T>::Map(T2 (*f)(T)) const {
    auto* result = new MutableArraySequence<T2>();
    for (int i = 0; i < GetLength(); ++i) result->Append(f(Get(i)));
    return result;
}

template <class T>
template <class T2>
Sequence<T2>* Sequence<T>::MapIndexed(T2 (*f)(T, int)) const {
    auto* result = new MutableArraySequence<T2>();
    for (int i = 0; i < GetLength(); ++i) result->Append(f(Get(i), i));
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::Where(bool (*predicate)(T)) const {
    auto* result = new MutableArraySequence<T>();
    for (int i = 0; i < GetLength(); ++i)
        if (predicate(Get(i))) result->Append(Get(i));
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::FlatMap(Sequence<T>* (*f)(T)) const {
    auto* result = new MutableArraySequence<T>();
    for (int i = 0; i < GetLength(); ++i) {
        Sequence<T>* sub = f(Get(i));
        for (int j = 0; j < sub->GetLength(); ++j) result->Append(sub->Get(j));
        delete sub;
    }
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::Slice(int i, int n, Sequence<T>* s) const {
    int len    = GetLength();
    int actual = (i < 0) ? len + i : i;
    if (actual < 0 || actual > len)
        throw IndexOutOfRange("Slice: index " + std::to_string(i) +
                              " out of range for length " + std::to_string(len));
    if (n < 0) throw IndexOutOfRange("Slice: count cannot be negative");
    auto* result = new MutableArraySequence<T>();
    for (int k = 0; k < actual; ++k)                result->Append(Get(k));
    if (s) for (int k = 0; k < s->GetLength(); ++k) result->Append(s->Get(k));
    for (int k = actual + n; k < len; ++k)          result->Append(Get(k));
    return result;
}

template <class T>
Sequence<Sequence<T>*>* Sequence<T>::Split(bool (*isSeparator)(T)) const {
    auto* result  = new MutableArraySequence<Sequence<T>*>();
    auto* current = new MutableArraySequence<T>();
    for (int i = 0; i < GetLength(); ++i) {
        if (isSeparator(Get(i))) {
            result->Append(static_cast<Sequence<T>*>(current));
            current = new MutableArraySequence<T>();
        } else {
            current->Append(Get(i));
        }
    }
    result->Append(static_cast<Sequence<T>*>(current));
    return static_cast<Sequence<Sequence<T>*>*>(result);
}

// Zip / Unzip

template <class T1, class T2>
Sequence<Tuple<T1,T2>>* Zip(Sequence<T1>* a, Sequence<T2>* b) {
    int len = (a->GetLength() < b->GetLength()) ? a->GetLength() : b->GetLength();
    auto* result = new MutableArraySequence<Tuple<T1,T2>>();
    for (int i = 0; i < len; ++i)
        result->Append(Tuple<T1,T2>(a->Get(i), b->Get(i)));
    return result;
}

template <class T1, class T2>
Tuple<Sequence<T1>*, Sequence<T2>*> Unzip(Sequence<Tuple<T1,T2>>* seq) {
    auto* first  = new MutableArraySequence<T1>();
    auto* second = new MutableArraySequence<T2>();
    for (int i = 0; i < seq->GetLength(); ++i) {
        first->Append(seq->Get(i).first);
        second->Append(seq->Get(i).second);
    }
    return Tuple<Sequence<T1>*, Sequence<T2>*>(first, second);
}
