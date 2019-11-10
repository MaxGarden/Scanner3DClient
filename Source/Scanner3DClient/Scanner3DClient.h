#pragma once

#include <assert.h>
#include <memory>

namespace Scanner3DClient
{
    using byte = unsigned char;

    #define CLIENT_ASSERT(expression) assert(expression)

    #define DECLARE_POINTERS(x)                                 \
        class x;                                                \
        using x##SharedPtr = std::shared_ptr<x>;                \
        using x##ConstSharedPtr = std::shared_ptr<const x>;     \
        using x##WeakPtr = std::weak_ptr<x>;                    \
        using x##ConstWeakPtr = std::weak_ptr<const x>;         \
        using x##UniquePtr = std::unique_ptr<x>;                \
        using x##ConstUniquePtr = std::unique_ptr<const x>;     \

    namespace GUI
    {
        DECLARE_POINTERS(ClientView);
        DECLARE_POINTERS(ServicesView);
    }
}