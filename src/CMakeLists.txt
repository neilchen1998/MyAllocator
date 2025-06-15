file(GLOB MATH_HEADER_LIST CONFIGURE_DEPENDS "${CUSTOM_ALLOCATOR_SOURCE_DIR}/include/math/*.hpp")

add_library(my_allocator_library allocator.cc ${MATH_HEADER_LIST})

target_include_directories(my_allocator_library PUBLIC ../include)

# target_link_libraries(node_library PUBLIC my_allocator_library)

target_compile_features(my_allocator_library PUBLIC cxx_std_20)  # requires C++20 for concept
