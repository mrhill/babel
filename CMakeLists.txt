cmake_minimum_required (VERSION 2.6)
project (babel)

include_directories (include/babel)
file(GLOB HEADERS "include/babel/*.h")
file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB C_SOURCES "src/*.c")

add_library (babel ${C_SOURCES} ${CPP_SOURCES} ${HEADERS})

IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

# add_subdirectory(test)
