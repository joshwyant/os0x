#pragma once

#include "framebuf.h"
#include "mbedfont.h"

namespace k
{
    class SimpleCharacterRenderer
    {
    public:
        SimpleCharacterRenderer(FrameBuffer *buf)
            : buf_{buf} {}

        const FrameBuffer &frameBuffer() const { return *buf_; }

        void drawCharacter(char c, size_t x, size_t y, uint32_t bgcolor, uint32_t fgcolor)
        {
            if (c < 0)
                return;

            auto *p = buf_->pixels() + y * buf_->stride() + x;

            for (auto cy = 0; cy < kFont8x8Height; cy++)
            {
                const auto &charLine = font8x8_basic[c][cy];
                for (auto cx = 0; cx < kFont8x8Width; cx++)
                {
                    const auto bit = 0x80 >> cx;
                    p[cx] = bit & charLine ? fgcolor : bgcolor;
                }

                p += buf_->stride();
            }
        }

    protected:
        FrameBuffer *buf_;
    }; // class SimpleCharacterRenderer

    enum class ConsoleColor : uint8_t
    {
        Black,
        DarkBlue,
        DarkGreen,
        DarkCyan,
        DarkRed,
        DarkMagenta,
        DarkYellow,
        Gray,
        DarkGray,
        Blue,
        Green,
        Cyan,
        Red,
        Magenta,
        Yellow,
        White
    };

    class SimpleTextOutput
    {
    public:
        SimpleTextOutput(SimpleCharacterRenderer *renderer)
            : renderer_{renderer},
              rows_{renderer->frameBuffer().height() / kFont8x8Height},
              cols_{renderer->frameBuffer().width() / kFont8x8Width},
              row_{0},
              col_{0},
              bg_{ConsoleColor::Black},
              fg_{ConsoleColor::White}
        {
        }

        ConsoleColor bg() const { return bg_; }
        ConsoleColor fg() const { return fg_; }
        void setBg(ConsoleColor c)
        {
            if (static_cast<uint8_t>(c) < 15)
                bg_ = c;
        }
        void setFg(ConsoleColor c)
        {
            if (static_cast<uint8_t>(c) < 15)
                fg_ = c;
        }

        void print(const char *chars)
        {
            // while not terminating zero
            while (*chars)
            {
                // current char
                const auto c = *chars;
                // Wrap or break
                if (c == '\n' || col_ >= cols_)
                {
                    col_ = 0;
                    row_++;
                }
                // Scroll up one
                if (row_ >= rows_)
                {
                    auto &fb = renderer_->frameBuffer();
                    auto bgcol = pallette_[static_cast<uint8_t>(bg_)];
                    for (auto y = 0; y < fb.height(); y++)
                    {
                        auto *from = fb.pixels() + fb.stride() * (y + kFont8x8Height);
                        auto *to = fb.pixels() + fb.stride() * y;
                        if (y + kFont8x8Height < fb.height())
                        {
                            for (auto x = 0; x < fb.width(); x++)
                            {
                                *(to++) = *(from++);
                            }
                        }
                        else
                        {
                            for (auto x = 0; x < fb.width(); x++)
                                *(to++) = bgcol;
                        }
                    }
                }

                if (c == '\t')
                {
                    auto newCol = (col_ + 7) / 8 * 8; // round column up to next 8
                    // Draw spaces
                    for (; col_ < newCol && col_ < cols_; col_++)
                    {
                        drawChar(' ');
                    }
                }
                else if (c >= 32)
                {
                    drawChar(c);
                }
                chars++;
            }
        }

    protected:
        SimpleCharacterRenderer *const renderer_;
        const size_t rows_;
        const size_t cols_;
        size_t row_;
        size_t col_;
        ConsoleColor bg_;
        ConsoleColor fg_;
        void drawChar(char c)
        {
            renderer_->drawCharacter(
                c,
                col_ * kFont8x8Width,
                row_ * kFont8x8Height,
                pallette_[static_cast<uint8_t>(bg_)],
                pallette_[static_cast<uint8_t>(fg_)]);
        }

    private:
        // TODO: make static later
        // // https://lospec.com/palette-list/microsoft-vga
        // const uint32_t pallette_[16] = {
        //     0x000000, // black
        //     0x000080, // dark blue
        //     0x008000, // dark green
        //     0x008080, // dark cyan
        //     0x800000, // dark red
        //     0x800080, // dark magenta
        //     0x808000, // dark yellow
        //     0x808080, // dark gray
        //     0xc0c0c0, // gray
        //     0x0000ff, // blue
        //     0x00ff00, // green
        //     0x00ffff, // cyan
        //     0xff0000, // red
        //     0xff00ff, // magenta
        //     0xffff00, // yellow
        //     0xffffff, // white
        // };

        // Catppuccin mocha
        // https://catppuccin.com/palette/
        const uint32_t pallette_[16] = {
            0x181825, // black:         mantle
            0xb4befe, // dark blue:     lavender
            0x94e2d5, // dark green:    teal
            0x89dceb, // dark cyan:     sky
            0xeba0ac, // dark red:      maroon
            0xcba6f7, // dark magenta:  mauve
            0xfab387, // dark yellow:   peach
            0x7f849c, // gray:          overlay 1
            0x313244, // dark gray:     surface 0
            0x89b4fa, // blue:          blue
            0xa6e3a1, // green:         green
            0x74c7ec, // cyan:          sapphire
            0xf38ba8, // red:           red
            0xf5c2e7, // magenta:       pink
            0xf9e2af, // yellow:        yellow
            0xcdd6f4, // white:         text
        };
    };

}
