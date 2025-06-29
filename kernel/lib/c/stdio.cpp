#include "stdio.h"
#include <stdint.h>
#include <stdarg.h>

// https://cplusplus.com/reference/cstdio/printf/
void Formatter::parsef(const char *format, ...) const
{
    va_list arglist;
    format_ = format;
    c_ = format;
    uintmax_t widened;
    intmax_t widenedSigned;

    va_start(arglist, format);

    while (*c_)
    {
        if (*c_ == '%')
        {
            c_++;
            parseFlags();
            if (parseWidth() == -1)
            {
                width_ = va_arg(arglist, int);
            }
            parsePrecision();
            parseLength();
        }
        switch (*c_)
        {
        case 'd':
        case 'i':
            // signed decimal integer
            c_++;
            switch (size_)
            {
            case sizeof(int64_t):
                widenedSigned = va_arg(arglist, int64_t);
                break;
            case sizeof(int8_t):  // automatically promoted to int in varargs
            case sizeof(int16_t): //
            case sizeof(int32_t):
            default:
                widenedSigned = static_cast<intmax_t>(va_arg(arglist, int32_t));
                break;
            }
            outputInteger(static_cast<uintmax_t>(widenedSigned), true);
            break;
        case 'u':
            // unsigned decimal integer
        case 'o':
            // unsigned octal
        case 'x':
            // unsigned hexadecimal integer
        case 'X':
            // unsigned hexadecimal integer (uppercase)
        case 'p':
            // Pointer address
            switch (size_)
            {
            case sizeof(uint64_t):
                widened = va_arg(arglist, uint64_t);
                break;
            case sizeof(uint8_t):  // automatically promoted to int
            case sizeof(uint16_t): //
            case sizeof(uint32_t):
            default:
                widened = static_cast<uintmax_t>(va_arg(arglist, uint32_t));
                break;
            }
            switch (*(c_++))
            {
            case 'u':
                outputInteger(widened, false);
                break;
            case 'o':
                // Probably unsupported, fn will probably output a `?`
                outputOctal(widened);
                break;
            case 'X':
                outputHex(widened, true);
                break;
            default: // x, p
                outputHex(widened, false);
                break;
            }
            break;
        case 'f':
            // Decimal floating point, lowercase
        case 'e':
            // Scientific notation (mantissa/exponent), lowercase
        case 'E':
            // Scientific notation (mantissa/exponent), uppercase
        case 'g':
            // Use shortest representation: %e or %f
        case 'G':
            // Use shortest representation: %E or %F
        case 'a':
            // Hex floating point, lowercase
        case 'A':
            // Hex floating point, uppercase
            c_++;
            switch (size_)
            {
            case sizeof(long double):
                va_arg(arglist, long double);
                // Don't do anything: Unsupported
                break;
            default: // double
                va_arg(arglist, double);
                // Don't do anything with it; floating point unsupported.
                break;
            }
            outputChars("?", 1);
            break;
        case 'c':
            // Character
            c_++;
            switch (size_)
            {
            case sizeof(wchar_t):
                va_arg(arglist, wchar_t);
                // Don't do anything: Unsupported
                outputChars("?", 1);
                break;
            default: // char
                char c = va_arg(arglist, char);
                outputChars(&c, 1);
                break;
            }
            break;
        case 's':
            // String of characters
            c_++;
            switch (size_)
            {
            case sizeof(wchar_t):
                va_arg(arglist, wchar_t);
                // Don't do anything: Unsupported
                outputChars("?", 1);
                break;
            default: // char
                const char *str = va_arg(arglist, const char *);
                outputChars(str);
                break;
            }
            break;
        case 'n':
            // Nothing printed.
            // The corresponding argument must be a pointer to a signed int.
            // The number of characters written so far is stored in the pointed location.
            c_++;
            switch (size_)
            {
            case sizeof(int64_t):
                *va_arg(arglist, int64_t *) = static_cast<int64_t>(charsPrinted_);
                break;
            case sizeof(int8_t):
                *va_arg(arglist, int8_t *) = static_cast<int8_t>(charsPrinted_);
                break;
            case sizeof(int16_t):
                *va_arg(arglist, int16_t *) = static_cast<int16_t>(charsPrinted_);
                break;
            case sizeof(int32_t):
            default:
                *va_arg(arglist, int32_t *) = static_cast<int32_t>(charsPrinted_);
                break;
            }
            break;

        case '%':
            // Escape for %
            outputChars(c_++, 1);
            break;

        default:
            // What do we do here?
            // Maybe grab and discard the arg
            va_arg(arglist, int);
            outputChars("?", 1);
            break;
        }
    }

    va_end(arglist);
}

void Formatter::parseFlags() const
{
    flags_ = FormatFlags::None;
    while (true)
    {
        switch (*c_)
        {
        case '-':
            flags_ |= FormatFlags::LeftJustify;
            c_++;
            break;
        case '+':
            flags_ |= FormatFlags::PlusPrefix;
            c_++;
            break;
        case ' ':
            flags_ |= FormatFlags::Space;
            c_++;
            break;
        case '#':
            flags_ |= FormatFlags::Hash;
            c_++;
            break;
        case '0':
            flags_ |= FormatFlags::Zero;
            c_++;
            break;
        default:
            return;
        }
    }
}

int Formatter::parseWidth() const
{
    if (*c_ == '*')
    {
        c_++;
        return -1; // get from vararg instead
    }

    auto width = 0;
    while (*c_ >= '0' && *c_ <= '9')
    {
        width = width * 10 + (*c_ - '0');
        c_++;
    }
    return width;
}

void Formatter::parsePrecision() const
{
    precision_ = 0;
    if (*c_ != '.')
    {
        return;
    }
    c_++;

    while (*c_ >= '0' && *c_ <= '9')
    {
        precision_ = precision_ * 10 + (*c_ - '0');
        c_++;
    }
}

void Formatter::parseLength() const
{
    switch (*c_)
    {
    case 'h':
        c_++;
        if (*c_ == 'h')
        {
            c_++;
            size_ = sizeof(unsigned char);
        }
        else
        {
            size_ = sizeof(unsigned short int);
        }
        break;
    case 'l':
        c_++;
        if (*c_ == 'l')
        {
            c_++;
            size_ = sizeof(unsigned long long int);
        }
        else
        {
            size_ = sizeof(long);
        }
        break;
    case 'j':
        c_++;
        size_ = sizeof(uintmax_t);
        break;
    case 'z':
        c_++;
        size_ = sizeof(size_t);
        break;
    case 't':
        c_++;
        size_ = sizeof(ptrdiff_t);
        break;
    case 'L':
        c_++;
        size_ = sizeof(long double);
        break;
    default:
        size_ = sizeof(unsigned int);
        break;
    }
}

void StringFormatter::outputChars(const char *chars, size_t length) const
{
}
