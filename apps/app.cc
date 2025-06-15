#include <cstddef>
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <fmt/core.h>   // fmt::print
#include <memory_resource>  // std::pmr::monotonic_buffer_resource
#include <span>

#include "allocator/allocator.hpp"
#include "print/print.hpp"

int main(int argc, char* argv[])
{
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 20;

    std::array<char, bufferSz> buf;
    std::array<char, backUpSz> upstream;

    auto upst = std::pmr::monotonic_buffer_resource {upstream.data(), upstream.size()};
    auto pool = std::pmr::monotonic_buffer_resource {buf.data(), buf.size(), &upst};
    std::pmr::vector<char> vec(&pool);

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    fmt::print("=== First wave of pushes ===\n");
    for (auto c = 'a'; c <= 'h'; ++c)
    {
        vec.emplace_back(c);
        myVec.emplace_back(c);
    }

    fmt::print("buffer:\n");
    println(std::span<char>(buf), std::span<char>(buffer));
    fmt::print("back up:\n");
    println(std::span<char>(upstream), std::span<char>(backUp));
    fmt::print("vec:\n");
    println(std::span<char>(vec), std::span<char>(myVec));

    for (auto c = 'a'; c <= 'z'; ++c)
    {
        vec.emplace_back(c);
        myVec.emplace_back(c);
    }

    vec.clear();
    vec.shrink_to_fit();
    myVec.clear();
    primaryAlloc.clear();
    upstreamAlloc.clear();
    myVec.shrink_to_fit();

    fmt::print("=== After clear ===\n");
    fmt::print("buffer:\n");
    println(std::span<char>(buf), std::span<char>(buffer));
    fmt::print("back up:\n");
    println(std::span<char>(upstream), std::span<char>(backUp));
    fmt::print("vec:\n");
    println(std::span<char>(vec), std::span<char>(myVec));

    fmt::print("=== Second wave of pushes ===\n");
    for (auto c = 'z'; c >= 'y'; --c)
    {
        vec.push_back(c);
        myVec.push_back(c);
    }

    fmt::print("buffer:\n");
    println(std::span<char>(buf), std::span<char>(buffer));
    fmt::print("back up:\n");
    println(std::span<char>(upstream), std::span<char>(backUp));
    fmt::print("vec:\n");
    println(std::span<char>(vec), std::span<char>(myVec));

    return EXIT_SUCCESS;
}
