#pragma once
#include "Exceptions.h"
#include "IEnumerable.h"

// Однонаправленный связный список
template <class T>
class LinkedList : public IEnumerable<T> {
    struct Node {
        T     data;
        Node* next;
        explicit Node(T d) : data(std::move(d)), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    int   length_;

    void Clear() {
        Node* cur = head_;
        while (cur) { Node* n = cur->next; delete cur; cur = n; }
        head_ = tail_ = nullptr;
        length_ = 0;
    }

public:
    // конструкторы
    LinkedList(T* items, int count) : head_(nullptr), tail_(nullptr), length_(0) {
        for (int i = 0; i < count; ++i) Append(items[i]);
    }

    LinkedList() : head_(nullptr), tail_(nullptr), length_(0) {}

    LinkedList(const LinkedList<T>& other) : head_(nullptr), tail_(nullptr), length_(0) {
        for (Node* cur = other.head_; cur; cur = cur->next) Append(cur->data);
    }

    LinkedList& operator=(const LinkedList<T>& other) {
        if (this == &other) return *this;
        Clear();
        for (Node* cur = other.head_; cur; cur = cur->next) Append(cur->data);
        return *this;
    }

    ~LinkedList() { Clear(); }

    // методы доступа
    T GetFirst() const {
        if (!head_) throw IndexOutOfRange("LinkedList::GetFirst: list is empty");
        return head_->data;
    }

    T GetLast() const {
        if (!tail_) throw IndexOutOfRange("LinkedList::GetLast: list is empty");
        return tail_->data;
    }

    T Get(int index) const {
        if (index < 0 || index >= length_)
            throw IndexOutOfRange(index, length_ - 1);
        Node* cur = head_;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    T& GetRef(int index) {
        if (index < 0 || index >= length_)
            throw IndexOutOfRange(index, length_ - 1);
        Node* cur = head_;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex >= length_ || startIndex > endIndex)
            throw IndexOutOfRange("LinkedList::GetSubList: invalid range [" +
                std::to_string(startIndex) + ", " + std::to_string(endIndex) + "]");
        auto* result = new LinkedList<T>();
        Node* cur = head_;
        for (int i = 0; i < startIndex; ++i) cur = cur->next;
        for (int i = startIndex; i <= endIndex; ++i, cur = cur->next)
            result->Append(cur->data);
        return result;
    }

    int GetLength() const { return length_; }

    // модифицирующие методы
    void Append(T item) {
        Node* node = new Node(std::move(item));
        if (!tail_) { head_ = tail_ = node; }
        else { tail_->next = node; tail_ = node; }
        ++length_;
    }

    void Prepend(T item) {
        Node* node = new Node(std::move(item));
        node->next = head_;
        head_ = node;
        if (!tail_) tail_ = head_;
        ++length_;
    }

    void InsertAt(T item, int index) {
        if (index < 0 || index > length_)
            throw IndexOutOfRange(index, length_);
        if (index == 0)        { Prepend(std::move(item)); return; }
        if (index == length_)  { Append(std::move(item));  return; }
        Node* prev = head_;
        for (int i = 0; i < index - 1; ++i) prev = prev->next;
        Node* node = new Node(std::move(item));
        node->next = prev->next;
        prev->next = node;
        ++length_;
    }

    LinkedList<T>* Concat(LinkedList<T>* other) const {
        auto* result = new LinkedList<T>(*this);
        for (Node* cur = other->head_; cur; cur = cur->next)
            result->Append(cur->data);
        return result;
    }

    // итератор
    class Enumerator : public IEnumerator<T> {
        Node* cur_;
        bool  started_;
        const LinkedList<T>* list_;
    public:
        explicit Enumerator(const LinkedList<T>* l)
            : cur_(nullptr), started_(false), list_(l) {}
        bool MoveNext() override {
            if (!started_) { cur_ = list_->head_; started_ = true; }
            else if (cur_) cur_ = cur_->next;
            return cur_ != nullptr;
        }
        T    Current() const override {
            if (!cur_) throw IndexOutOfRange("LinkedList::Enumerator: no current");
            return cur_->data;
        }
        void Reset() override { cur_ = nullptr; started_ = false; }
    };

    IEnumerator<T>* GetEnumerator() const override { return new Enumerator(this); }

    // range-based for
    struct Iterator {
        Node* node;
        explicit Iterator(Node* n) : node(n) {}
        T&        operator*()  { return node->data; }
        Iterator& operator++() { node = node->next; return *this; }
        bool      operator!=(const Iterator& o) const { return node != o.node; }
    };
    Iterator begin() { return Iterator(head_); }
    Iterator end()   { return Iterator(nullptr); }
};
