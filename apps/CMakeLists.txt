add_executable(app app.cc)
target_compile_features(app PRIVATE cxx_std_20)

# required libraries
target_link_libraries(app PRIVATE fmt::fmt my_allocator_library print_library)
