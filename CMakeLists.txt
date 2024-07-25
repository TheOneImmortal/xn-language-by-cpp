cmake_minimum_required(VERSION 3.28)

project(XN)

set(CMAKE_GENERATOR "D:/Xn/Code/Engine/Python/Python311/Scripts/ninja")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source_files ${CMAKE_CURRENT_LIST_DIR}/src/*.cc)
file(GLOB_RECURSE current_headers ${CMAKE_CURRENT_LIST_DIR}/src/*.hh)

add_executable(runme ${source_files} ${current_headers})
