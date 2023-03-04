cmake_minimum_required(VERSION 3.14)

project(babel LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include_directories (include/babel)
file(GLOB HEADERS "include/babel/*.h")
file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB C_SOURCES "src/*.c")

add_library (babel STATIC ${C_SOURCES} ${CPP_SOURCES} ${HEADERS})
target_link_libraries(babel PRIVATE Qt${QT_VERSION_MAJOR}::Core)


#IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
#  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
#ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

# add_subdirectory(test)
