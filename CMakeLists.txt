cmake_minimum_required (VERSION 2.6)
project (babel)

include_directories (include/babel)
file(GLOB HEADERS "include/babel/*.h")
file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB C_SOURCES "src/*.c")
add_library (babel ${C_SOURCES} ${CPP_SOURCES} ${HEADERS})

# add_subdirectory(test)
