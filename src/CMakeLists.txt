# get all the source files
file(GLOB_RECURSE sources "*.cpp")

# add the executable
add_executable(server ${sources})

set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
