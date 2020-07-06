cmake_minimum_required (VERSION 3.8)

project(tl)


option(
  gtest_force_shared_crt
  "Use shared (DLL) run-time lib even when Google Test is built as static lib."
  ON)

enable_testing()


add_subdirectory(googletests)
add_subdirectory(tests)
add_subdirectory(tl)
