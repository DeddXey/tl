cmake_minimum_required (VERSION 3.8)

project(tests)


set(SRCS
    tests
    fifo_tst
)

add_executable(tests  ${SRCS})

target_compile_features(tests PRIVATE cxx_std_17)


target_include_directories(tests PUBLIC
    "../googletest/googletest/include"
    )

target_link_libraries(tests
    tl
    gtest)

