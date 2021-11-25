cmake_minimum_required(VERSION 3.10)

# set the project entityName
project(SimpleRA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

# add the src directory
add_subdirectory(src)