#pragma once

namespace k
{

    template <typename T>
    inline T &&move(T &a)
    {
        return static_cast<T &&>(a);
    }
    template <typename T>
    inline void swap(T &a, T &b)
    {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }
}