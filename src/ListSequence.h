#pragma once
#include "Sequence.h"
#include "LinkedList.h"

// Базовый класс на основе LinkedList
template <class T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T>* list_;

public:
    ListSequence()
        : list_(new LinkedList<T>()) {}

    ListSequence(T* items, int count)
        : list_(new LinkedList<T>(items, count)) {}

    ListSequence(const ListSequence<T>& other)
        : list_(new LinkedList<T>(*other.list_)) {}

    ~ListSequence() override { delete list_; }

    // чтение
    T   GetFirst() const override { return list_->GetFirst(); }
    T   GetLast()  const override { return list_->GetLast();  }
    T   Get(int index) const override { return list_->Get(index); }
    int GetLength()    const override { return list_->GetLength(); }

    // Объявление без определения — MutableListSequence ещё не известен
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;

    // внутренние мутации
    void AppendInPlace(T item)              { list_->Append(std::move(item));          }
    void PrependInPlace(T item)             { list_->Prepend(std::move(item));         }
    void InsertAtInPlace(T item, int index) { list_->InsertAt(std::move(item), index); }

    // мутация через Instance()
    Sequence<T>* Append(T item) override {
        auto* inst = static_cast<ListSequence<T>*>(this->Instance());
        inst->AppendInPlace(item);
        return inst;
    }
    Sequence<T>* Prepend(T item) override {
        auto* inst = static_cast<ListSequence<T>*>(this->Instance());
        inst->PrependInPlace(item);
        return inst;
    }
    Sequence<T>* InsertAt(T item, int index) override {
        auto* inst = static_cast<ListSequence<T>*>(this->Instance());
        inst->InsertAtInPlace(item, index);
        return inst;
    }
    Sequence<T>* Concat(Sequence<T>* other) override {
        auto* inst = static_cast<ListSequence<T>*>(this->Instance());
        for (int i = 0; i < other->GetLength(); ++i)
            inst->AppendInPlace(other->Get(i));
        return inst;
    }

    // операторы
    T&       operator[](int index)       { return list_->GetRef(index); }
    const T& operator[](int index) const {
        return const_cast<LinkedList<T>*>(list_)->GetRef(index);
    }

    // итератор
    IEnumerator<T>* GetEnumerator() const override {
        return list_->GetEnumerator();
    }
};

// MutableListSequence
template <class T>
class MutableListSequence : public ListSequence<T> {
public:
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    MutableListSequence(const MutableListSequence<T>& other) : ListSequence<T>(other) {}

    Sequence<T>* Clone()       const override { return new MutableListSequence<T>(*this); }
    Sequence<T>* Instance()          override { return this; }
    Sequence<T>* CreateEmpty() const override { return new MutableListSequence<T>(); }
};

// ImmutableListSequence
template <class T>
class ImmutableListSequence : public ListSequence<T> {
public:
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequence<T>(other) {}

    Sequence<T>* Clone()       const override { return new ImmutableListSequence<T>(*this); }
    Sequence<T>* Instance()          override { return this->Clone(); }
    Sequence<T>* CreateEmpty() const override { return new ImmutableListSequence<T>(); }
};

// GetSubsequence — определяем здесь, когда MutableListSequence уже известен
template <class T>
Sequence<T>* ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex)
        throw IndexOutOfRange("ListSequence::GetSubsequence: invalid range [" +
            std::to_string(startIndex) + ", " + std::to_string(endIndex) + "]");
    auto* result = new MutableListSequence<T>();
    for (int i = startIndex; i <= endIndex; ++i) result->AppendInPlace(Get(i));
    return result;
}
