#pragma once

#include <span> // std::span
#include <fmt/core.h>   // fmt::print

template<typename T, typename U>
void println(std::span<T> s1, std::span<U> s2)
{
    for (std::size_t i = 0; i < s1.size() && i < s2.size(); ++i)
    {
        fmt::print("#{:<2} => {} @{:#x} {} @{:#x}\n", i, s1[i], reinterpret_cast<uintptr_t>(&s1[i]), s2[i], reinterpret_cast<uintptr_t>(&s2[i]));
    }
}
