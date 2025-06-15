#include <cstddef>
#include <iostream> // std::cerr
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE

#include <fmt/core.h>   // fmt::print

int main(int argc, char* argv[])
{
    fmt::print("Hello, {}!\n", "world");

    return EXIT_SUCCESS;
}
