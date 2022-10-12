cmake_minimum_required(VERSION 3.16)

project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS program_options coroutine)

add_subdirectory(external/googletest)
add_subdirectory(src/common)
add_subdirectory(src/client)
add_subdirectory(src/server)
