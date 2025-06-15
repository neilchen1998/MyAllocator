# cmake requirements
cmake_minimum_required(VERSION 3.14)

project(
    CustomAllocator
    VERSION 0.1
    DESCRIPTION "Custom allocator for STL container"
    LANGUAGES CXX
)

# checks if we are in the main CMakeLists
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # ensures -std=c++xx instead of -std=g++xx for protability purposes
    set(CMAKE_CXX_EXTENSIONS OFF)

    # compiler flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Wall -Wextra -pedantic -g3 -fsanitize=address,undefined)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Test")
        add_compile_options(-Wall -Wextra -pedantic -O3)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Gprof")
        add_compile_options(-Wall -pg -g3)
    endif()

    # linker flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_link_options(-g3 -fsanitize=address,undefined)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Gprof")
        add_link_options(-pg -g3)
    endif()

    # uses CTest
    # NOTE: this needs to be done in the main CMakeLists
    include(CTest)

    # Docs only available in the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found")
    endif()
endif()

# FetchContent helps grabbing repos during the configure step
# NOTE: available after CMake 3.11
include(FetchContent)

# all required packages

# the compiled library code
add_subdirectory(src)

# the executable code
add_subdirectory(apps)

# checks if we are in the main CMakeLists
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING AND CMAKE_BUILD_TYPE STREQUAL "Test")
    add_subdirectory(tests)
endif()
