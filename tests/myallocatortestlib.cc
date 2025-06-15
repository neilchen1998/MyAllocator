#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <array>    // std::array
#include <memory_resource>  // std::pmr::monotonic_buffer_resource
#include <span> // std::span

#include "allocator/allocator.hpp"

TEST_CASE( "Elements Less Than Main Buffer", "[main]" )
{
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 20;
    constexpr std::size_t N = bufferSz - 1;

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (auto c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (auto i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }
}

TEST_CASE( "Elements More Than Main Buffer But Less Than the Upstream Buffer", "[main]" )
{
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 20;
    constexpr auto N = (backUpSz - 1);

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (auto c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (auto i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }
}

TEST_CASE( "Elements More Than the Upstream Buffer", "[main]" )
{
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 20;
    constexpr auto N = ('z' - 'a' + 1);

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (auto c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (auto i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }
}
