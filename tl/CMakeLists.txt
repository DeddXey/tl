cmake_minimum_required (VERSION 3.12)

project(tl)

#set(SRC
#    src/goertzel
#    src/utility
#    )
	
add_library(tl STATIC 
${SRC}
        include/sort/qsort.h include/sort/shell.h)

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
