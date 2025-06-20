#pragma once

#include <stddef.h>

inline void *operator new(size_t, void *ptr) noexcept { return ptr; }
inline void operator delete(void *, void *) noexcept {} // no-op

#define CHECK_STATUS()                 \
    do                                 \
    {                                  \
        if (status != status_code::ok) \
        {                              \
            return status;             \
        }                              \
    } while (0)

namespace k
{
    template <typename T>
    inline T &&move(T &a)
    {
        return static_cast<T &&>(a);
    }
    template <typename T>
    inline void swap(T &a, T &b) noexcept
    {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }

    enum class status_code
    {
        ok,
        unspecified,
        out_of_bounds,
    };
}