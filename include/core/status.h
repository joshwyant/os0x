#pragma once

#define CHECK_STATUS()                \
    do                                \
    {                                 \
        if (status != StatusCode::Ok) \
        {                             \
            return status;            \
        }                             \
    } while (0)

namespace k
{
    enum class [[nodiscard]] StatusCode
    {
        Ok,
        Unspecified,
        OutOfBounds,
        NotImplemented,
        OutOfMemory,
        OutOfRange,
        Uninitialized,
        InitializationError,
    }; // enum class StatusCode
} // namespace k