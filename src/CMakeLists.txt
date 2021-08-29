# get all the source files
file(GLOB_RECURSE sources "*.cpp")

# add the executable
add_executable(server ${sources})

#
#target_include_directories(server PUBLIC "${CMAKE_SOURCE_DIR}/src")
