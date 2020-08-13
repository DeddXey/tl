cmake_minimum_required (VERSION 3.12)

project(tl)

#find_package(Doxygen
#             REQUIRED dot
#             OPTIONAL_COMPONENTS mscgen dia)


option(
  gtest_force_shared_crt
  "Use shared (DLL) run-time lib even when Google Test is built as static lib."
  ON)

enable_testing()


add_subdirectory(googletest)
add_subdirectory(tests)
add_subdirectory(tl)

#set(DOXYGEN_RECURSIVE YES)
#set(DOXYGEN_EXCLUDE_PATTERNS
#    */.git/*
#    */.svn/*
#    */.hg/*
#    */CMakeFiles/*
#    */_CPack_Packages/*
#    DartConfiguration.tcl
#    */bld/*
#    */googletest/*
#    CMakeLists.txt
#    CMakeCache.txt)

#doxygen_add_docs(tldocs
#    ALL
#    )
