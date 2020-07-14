cmake_minimum_required (VERSION 3.8)

project(tl)

set(SRC
    src/goertzel
	src/utility
    )
	
add_library(tl STATIC 
${SRC}
)

target_include_directories (tl PUBLIC
    "include"
    )

#set(DOXYGEN_RECURSIVE YES)
#doxygen_add_docs(tldocs
#    ${PROJECT_SOURCE_DIR}
#    )
