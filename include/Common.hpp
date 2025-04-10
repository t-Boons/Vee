#pragma once

#include <memory>
#include <queue>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <string>
#include <any>
#include <exception>

#include "Log.hpp"
#include "Utils.hpp"

// Supress warning macros
#define VEE_WARNING_PUSH(x) __pragma(warning(push, x))
#define VEE_WARNING_POP     __pragma(warning(pop))

#define VEE_BIND_FUNC(name) std::bind(name, this, std::placeholders::_1)

#define FREE(p)         \
    {if (p) {            \
        delete p;       \
        p = nullptr;    \
    }}

namespace Vee
{
    template <typename T>
    using RefPtr = std::shared_ptr<T>;
    template <typename T>
    using ScopePtr = std::unique_ptr<T>;
    template <typename T>
    using WeakPtr = std::weak_ptr<T>;

    template <typename T, typename... Args>
    inline RefPtr<T> MakeRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    inline ScopePtr<T> MakeScope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    inline WeakPtr<T> MakeWeak(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
}