#pragma once
#include "Exceptions.h"
#include "IEnumerable.h"

// Вспомогательная функция минимума
template <class T>
const T& min(const T& a, const T& b) { return a < b ? a : b; }

// Динамический массив с управлением памятью
template <class T>
class DynamicArray : public IEnumerable<T> {
    T*  data_;
    int size_;

public:
    // конструкторы
    DynamicArray(T* items, int count) : size_(count) {
        if (count < 0) throw IndexOutOfRange("DynamicArray: negative count");
        data_ = new T[count];
        for (int i = 0; i < count; ++i) data_[i] = items[i];
    }

    explicit DynamicArray(int size) : size_(size) {
        if (size < 0) throw IndexOutOfRange("DynamicArray: negative size");
        data_ = new T[size]();
    }

    DynamicArray(const DynamicArray<T>& other) : size_(other.size_) {
        data_ = new T[size_];
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }

    DynamicArray& operator=(const DynamicArray<T>& other) {
        if (this == &other) return *this;
        delete[] data_;
        size_ = other.size_;
        data_ = new T[size_];
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
        return *this;
    }

    ~DynamicArray() { delete[] data_; }

    // методы доступа
    T Get(int index) const {
        if (index < 0 || index >= size_)
            throw IndexOutOfRange(index, size_ - 1);
        return data_[index];
    }

    int GetSize() const { return size_; }

    void Set(int index, T value) {
        if (index < 0 || index >= size_)
            throw IndexOutOfRange(index, size_ - 1);
        data_[index] = value;
    }

    // изменение размера
    void Resize(int newSize) {
        if (newSize < 0) throw IndexOutOfRange("DynamicArray::Resize: negative size");
        T* nd = new T[newSize]();
        int copyCount = min(size_, newSize);
        for (int i = 0; i < copyCount; ++i) nd[i] = data_[i];
        delete[] data_;
        data_ = nd;
        size_ = newSize;
    }

    // операторы
    T& operator[](int index) {
        if (index < 0 || index >= size_) throw IndexOutOfRange(index, size_ - 1);
        return data_[index];
    }
    const T& operator[](int index) const {
        if (index < 0 || index >= size_) throw IndexOutOfRange(index, size_ - 1);
        return data_[index];
    }

    // итератор
    class Enumerator : public IEnumerator<T> {
        const DynamicArray<T>* arr_;
        int pos_;
    public:
        explicit Enumerator(const DynamicArray<T>* a) : arr_(a), pos_(-1) {}
        bool MoveNext() override { return ++pos_ < arr_->size_; }
        T    Current()  const override {
            if (pos_ < 0 || pos_ >= arr_->size_)
                throw IndexOutOfRange(pos_, arr_->size_ - 1);
            return arr_->data_[pos_];
        }
        void Reset() override { pos_ = -1; }
    };

    IEnumerator<T>* GetEnumerator() const override { return new Enumerator(this); }

    // range-based for
    T*       begin()       { return data_; }
    T*       end()         { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end()   const { return data_ + size_; }
};
