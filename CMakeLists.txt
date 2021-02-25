cmake_minimum_required (VERSION 3.12)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tl)

#set(SRC
#    src/goertzel
#    src/utility
#    )
	
add_library(tl STATIC 
${SRC}
)

target_include_directories (tl PUBLIC
    "include"
    )

target_sources(tl
    PRIVATE     "src/goertzel"
                "src/utility"
                "include/utility"
                "include/view"
                "include/algo"
                "include/checksum"
                )
target_include_directories(tl
    PUBLIC "include/"
    )
