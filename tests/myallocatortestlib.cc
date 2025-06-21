#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <array>    // std::array
#include <memory_resource>  // std::pmr::monotonic_buffer_resource
#include <span> // std::span
#include <stdio.h> // printf

#include "allocator/allocator.hpp"

TEST_CASE( "Elements Less Than Main Buffer", "[main]" )
{
    // The size of the main buffer is larger than the number of elements
    // Therefore we do not need to reallocate space and all elements can be stored within the main buffer
    // NOTE: we need to consider the possibility of reallocating hence the size of the buffer is 4 times larger than that of the elements in this test case
    constexpr std::size_t bufferSz = 20;
    constexpr std::size_t backUpSz = 20;
    constexpr std::size_t N = bufferSz / 4;

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    // myVec.reserve(N);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }

    // Check the address of each element
    // Each address should be inside the main buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < 2; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p <= mainEndPtr);
        REQUIRE(isWithinMain);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE_FALSE(isWithinBackup);
    }
}

TEST_CASE( "Elements Less Than Main Buffer with Reserve", "[main]" )
{
    // The size of the main buffer is equal to the number of elements and we reserve with size N
    // Therefore we do not need to reallocate space and all elements can be stored within the main buffer
    constexpr std::size_t bufferSz = 20;
    constexpr std::size_t backUpSz = 20;
    constexpr std::size_t N = 20;

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    myVec.reserve(N);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }

    // Check the address of each element
    // Each address should be inside the main buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < 2; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p <= mainEndPtr);
        REQUIRE(isWithinMain);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE_FALSE(isWithinBackup);
    }
}

TEST_CASE( "Elements Less Than Main Buffer without Reserve", "[main]" )
{
    // The size of the main buffer is equal to the number of elements and we reserve with size N
    // Therefore we do not need to reallocate space and all elements can be stored within the main buffer
    // This example is the same as the previous one without calling the reserve function
    constexpr std::size_t bufferSz = 10;
    constexpr std::size_t backUpSz = 20;
    constexpr std::size_t N = bufferSz / 2;

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }

    // Check the address of each element
    // Each address should be inside the upstream buffer and NOT the main buffer
    // Since 1+2+...+5 = 15 and that is greater than the size of the main buffer if the allocator reallocates every time (worst case)
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < 2; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p <= mainEndPtr);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE_FALSE(isWithinMain);
        REQUIRE(isWithinBackup);
    }
}

TEST_CASE( "Elements More Than Main Buffer But Less Than the Upstream Buffer", "[main]" )
{
    // When we are keep pushing elements into the buffer, the allocator might need to reallocate over and over again
    // Therefore we need to have more than 26 char spaces so the allocator can reallocate and we will not need to allocate on the heap
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 1024;
    constexpr auto N = ('z' - 'a' + 1);

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }

    // Check the address of each element
    // Each address should be inside the upstream buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < N; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p < mainEndPtr);
        REQUIRE_FALSE(isWithinMain);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE(isWithinBackup);
    }
}

TEST_CASE( "Elements More Than the Upstream Buffer", "[main]" )
{
    // The number of elements is significantly greater than the size of the main and upstream buffer
    // Henceforth, the default allocator will come to rescue
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 20;
    constexpr std::size_t N = ('z' - 'a' + 1);

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == s.at(i));
    }

    // Check the address of each element
    // Each address should NOT be either in the main or the upstream buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < N; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p < mainEndPtr);
        REQUIRE_FALSE(isWithinMain);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE_FALSE(isWithinBackup);
    }
}

TEST_CASE( "Clear & Push Small Size Elements", "[main]" )
{
    // Calling clear after pushing elements into the main buffer without exceeding the main buffer
    // will reset the pointer
    // Then next time the elements will be at the beginning of the main buffer
    constexpr std::size_t bufferSz = 10;
    constexpr std::size_t backUpSz = 20;
    constexpr std::size_t N = 3;
    constexpr char offsetChar = 'k';

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    myVec.clear();

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + offsetChar);
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        const char c = (offsetChar + i);
        REQUIRE(myVec.at(i) == c);
    }

    // Check the address of each element
    // Each address should NOT be either in the main or the upstream buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < 2; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p <= mainEndPtr);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE(isWithinMain);
        REQUIRE_FALSE(isWithinBackup);
    }
}

TEST_CASE( "Clear & Push Medium Size Elements", "[main]" )
{
    // Calling clear after pushing elements into the main and upstream buffer
    // will reset the pointer
    // Then next time the elements will be at the beginning of the main buffer
    constexpr std::size_t bufferSz = 4;
    constexpr std::size_t backUpSz = 1024;
    constexpr auto N = ('z' - 'a' + 1);
    constexpr char offsetChar = 'e';

    std::array<char, bufferSz> buffer;
    std::array<char, backUpSz> backUp;

    MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
    MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
    std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);

    std::vector<char> s(N);
    std::iota(s.begin(), s.end(), 'a');

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    myVec.clear();

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + offsetChar);
    }

    REQUIRE(myVec.size() == N);

    for (std::size_t i {0}; i < N; ++i)
    {
        char c = offsetChar + i;
        REQUIRE(myVec.at(i) == c);
    }

    // Check the address of each element
    // Each address should be in the upstream buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < 2; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p <= mainEndPtr);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE_FALSE(isWithinMain);
        REQUIRE(isWithinBackup);
    }
}

TEST_CASE( "Clear & Push Large Size Elements", "[main]" )
{
    // When we are keep pushing elements into the buffer, the allocator might need to reallocate over and over again
    // Therefore we need to have more than 26 char spaces so the allocator can reallocate and we will not need to allocate on the heap
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

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back(c + 'a');
    }

    myVec.clear();

    for (std::size_t c = 0; c < N; ++c)
    {
        myVec.emplace_back('z' - c);
    }

    for (std::size_t i {0}; i < N; ++i)
    {
        REQUIRE(myVec.at(i) == static_cast<char>('z' - i));
    }

    // Check the address of each element
    // Each address should be in the upstream buffer
    auto* mainStartPtr = &buffer[0];
    auto* mainEndPtr = &buffer[bufferSz - 1];
    auto* backupStartPtr = &backUp[0];
    auto* backupEndPtr = &backUp[backUpSz - 1];
    for (std::size_t i {0}; i < 2; ++i)
    {
        auto* p = &myVec[i];
        bool isWithinMain = (p >= mainStartPtr && p <= mainEndPtr);
        bool isWithinBackup = (p >= backupStartPtr && p < backupEndPtr);
        REQUIRE_FALSE(isWithinMain);
        REQUIRE_FALSE(isWithinBackup);
    }
}
