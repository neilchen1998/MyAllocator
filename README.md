# Custom Allocator

## Summary

This is a custom light-weight memory allocator in C++ designed to achieve fast memory allocations in performance critical scenarios,
such as games or embedded systems.

To construct a `std::vector` with `MyBufferAllocator` as the allocator, one can provide a initial buffer as well as an additional upstream buffer.
When the initial buffer is exhausted, the upstream buffer will be used.
If the upstream buffer is also exhausted or simply not provided, the allocator will then fall back to the default allocator that allocates
memory on the heap.

This project is inspired by `std::pmr::monotonic_buffer_resource` which was introduced in C++17.

## Requirements

The requirements are:

- CMake 3.11 or better; 3.14+ highly recommended
- A C++17 compatible compiler
- Git
- Doxygen (optional, highly recommended)

## Instructions

To configure:

```bash
cmake -S . -B build
```

Add `-GNinja` if you have Ninja.

To build:

```bash
cmake --build build
```

To test (`--target` can be written as `-t` in CMake 3.15+):

```bash
cmake --build build --target test
```

To run exe
```
./build/apps/app
```

To build and run exe
```
cmake --build build && ./build/apps/app
```

To build and test
```
cmake --build build && cmake --build build --target test
```

To build docs (requires Doxygen, output in `build/docs/html`):

```bash
cmake --build build --target docs
```


## Example

To construct a `std::vector` with a main buffer:

```cpp
#include <array>
#include <vector>
#include "allocator/allocator.hpp"

std::array<char, 1024> buffer;

MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size());
std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);
```

To construct a `std::vector` with a main buffer and an upstream (backup) buffer:

```cpp
#include <array>
#include <vector>
#include "allocator/allocator.hpp"

std::array<char, 1024> buffer;
std::array<char, 2048> backUp;

MyBufferAllocator<char> upstreamAlloc (reinterpret_cast<std::byte*>(backUp.data()), backUp.size());
MyBufferAllocator<char> primaryAlloc (reinterpret_cast<std::byte*>(buffer.data()), buffer.size(), &upstreamAlloc);
std::vector<char, MyBufferAllocator<char>> myVec(primaryAlloc);
```

## License

`MyAllocator` is distributed under [GNU GENERAL PUBLIC LICENSE](https://github.com/neilchen1998/MyAllocator/blob/main/LICENSE).
