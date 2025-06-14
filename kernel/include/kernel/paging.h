#pragma once
#include "kernel/lib/cpp/utility.h"

class PageAllocator
{
public:
    PageAllocator()
    {
        t = 0;
    }
    ~PageAllocator() {}
    PageAllocator(const PageAllocator &other)
    {
        *this = other;
    }
    inline PageAllocator &operator=(const PageAllocator &other)
    {
        if (this != &other)
        {
            t = other.t;
        }
        return *this;
    }
    inline PageAllocator(PageAllocator &&other) noexcept
    {
        swap(*this, other);
    }
    inline PageAllocator &operator=(PageAllocator &&other) noexcept
    {
        if (this != &other)
        {
            swap(*this, other);
        }
        return *this;
    }

    friend void swap(PageAllocator &a, PageAllocator &b) noexcept
    {
        using k::swap;
        swap(a, b);
    }

private:
    int t;
};