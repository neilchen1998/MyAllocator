FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        6.1.2)
FetchContent_MakeAvailable(fmt)

add_executable(app app.cc)
target_compile_features(app PRIVATE cxx_std_20)

# required libraries
target_link_libraries(app PRIVATE fmt::fmt)
