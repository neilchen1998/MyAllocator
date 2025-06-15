file(GLOB ALLOCATOR_HEADER_LIST CONFIGURE_DEPENDS "${CUSTOM_ALLOCATOR_SOURCE_DIR}/include/allocator/*.hpp")
file(GLOB PRINT_HEADER_LIST CONFIGURE_DEPENDS "${CUSTOM_ALLOCATOR_SOURCE_DIR}/include/print/*.hpp")

add_library(my_allocator_library allocator.cc ${MATH_HEADER_LIST})
add_library(print_library print.cc ${PRINT_HEADER_LIST})

target_include_directories(my_allocator_library PUBLIC ../include)
target_include_directories(print_library PUBLIC ../include)

target_link_libraries(print_library PUBLIC fmt::fmt)

target_compile_features(my_allocator_library PUBLIC cxx_std_17)
target_compile_features(print_library PUBLIC cxx_std_20)
