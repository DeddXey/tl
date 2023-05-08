cmake_minimum_required (VERSION 3.18)

project(tl VERSION 0.9.9 DESCRIPTION "Small template library" LANGUAGES CXX)


#include(FetchContent)
#FetchContent_Declare(
#        doctest
#        GIT_REPOSITORY https://github.com/doctest/doctest.git
#)
#FetchContent_MakeAvailable(doctest)

#add_subdirectory(doctest)

add_library(tl STATIC )
target_sources(tl INTERFACE
        FILE_SET tl_includes TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE src/goertzel.cpp
        )
target_include_directories(tl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(tl PUBLIC cxx_std_20)


# for standalone testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    include(ExternalProject)
    find_package(Git REQUIRED)

    ExternalProject_Add(
            doctest
            PREFIX ${CMAKE_BINARY_DIR}/doctest
            GIT_REPOSITORY https://github.com/doctest/doctest.git
            TIMEOUT 10
            UPDATE_COMMAND ${GIT_EXECUTABLE} pull
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            LOG_DOWNLOAD ON
    )

    # Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
    ExternalProject_Get_Property(doctest source_dir)
    set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")
#    add_library(doctest INTERFACE)

    add_executable(test_runner
#            tests/f_parser_test.cpp
            tests/floatstatictest.cpp
            tests/utils_tst.cpp
            tests/statcnt_tst.cpp
            tests/main.cpp
            tests/wake_test.cpp include/wake/wake.h include/wake/packet_rc.h)
    add_dependencies(test_runner doctest)
    target_include_directories(test_runner
            PUBLIC ${DOCTEST_INCLUDE_DIR}
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(test_runner PUBLIC cxx_std_20)
    target_link_libraries(test_runner INTERFACE doctest)
#    add_test(NAME utility COMMAND test_runner)

endif()

#set(TEST_TARGET ${PROJECT_NAME}test)
#add_executable(${TEST_TARGET}
#        tests/main.cpp
#        tests/f_parser_test.cpp)
#
#
##target_link_libraries(${PROJECT_NAME} PRIVATE doctest)
#target_include_directories(${TEST_TARGET} PRIVATE
#        doctest/doctest
#        include)
#
#target_compile_features(${TEST_TARGET} PRIVATE cxx_std_20)
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#


install(TARGETS tl PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
