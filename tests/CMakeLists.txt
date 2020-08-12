cmake_minimum_required (VERSION 3.18)

project(tests)

add_executable(tests)

target_sources(tests
    PRIVATE tests
            fifo_tst
            utils_tst
            statcnt_tst
    )


target_include_directories(tests PUBLIC
    "../googletest/googletest/include"
    )

target_link_libraries(tests
    tl
    gtest)

target_compile_features(tests
    PRIVATE cxx_std_17
    )

#target_link_options(tests
#    PRIVATE "/MAP:${PROJECT_BINARY_DIR}/tests.map"
#    )
