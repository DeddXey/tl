cmake_minimum_required (VERSION 3.12)


project(tl VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(doctest)

add_executable(${PROJECT_NAME}
        tests/main.cpp
        tests/f_parser_test.cpp)


#target_link_libraries(${PROJECT_NAME} PRIVATE doctest)
target_include_directories(${PROJECT_NAME} PRIVATE
        doctest/doctest
        include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

