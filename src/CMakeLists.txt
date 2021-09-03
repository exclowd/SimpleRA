# get all the source files
file(GLOB_RECURSE sources "*.cpp")

# add the executable
add_executable(server ${sources} matrix.cpp matrix.h)
