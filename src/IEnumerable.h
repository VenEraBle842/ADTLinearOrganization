#pragma once
#include "IEnumerator.h"

// Интерфейс перечисляемой коллекции
template <class T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};
