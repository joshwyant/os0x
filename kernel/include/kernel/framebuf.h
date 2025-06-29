#pragma once

#include <stdint.h>
#include <stddef.h>

namespace k
{
    class FrameBuffer
    {
    public:
        FrameBuffer(uint32_t *pixels, size_t width, size_t height, size_t stride)
            : pixels_{pixels},
              width_{width},
              height_{height},
              stride_{stride}
        {
        }

        uint32_t *pixels() const { return pixels_; }
        size_t width() const { return width_; }
        size_t height() const { return height_; }
        size_t stride() const { return stride_; }

        void setPixel(size_t x, size_t y, uint32_t color)
        {
            pixels_[y * stride_ + x] = color;
        }

        void clear(uint32_t color)
        {
            for (int y = 0; y < height_; y++)
            {
                uint32_t *p = (pixels_ + y * stride_);
                for (int x = 0; x < width_; x++)
                {
                    p[x] = color;
                }
            }
        }

    protected:
        uint32_t *const pixels_;
        const size_t width_;
        const size_t height_;
        const size_t stride_;
    }; // class FrameBuffer
}
