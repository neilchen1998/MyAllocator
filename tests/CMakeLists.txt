FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(myallocatortestlib myallocatortestlib.cc)

target_compile_features(myallocatortestlib PRIVATE cxx_std_17)

target_link_libraries(myallocatortestlib PRIVATE Catch2::Catch2 my_allocator_library)

add_test(NAME myallocatortestlibtest COMMAND myallocatortestlib)
