#pragma once

// Интерфейс итератора
template <class T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;
    virtual bool MoveNext() = 0;
    virtual T    Current()  const = 0;
    virtual void Reset()    = 0;
};
