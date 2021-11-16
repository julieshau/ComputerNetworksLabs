cmake_minimum_required (VERSION 3.1)

set(CMAKE_C_FLAGS "-g -Wall")

# http://stackoverflow.com/questions/10555706/
macro (add_executable _name)
  _add_executable(${ARGV})
  if (TARGET ${_name})
    target_link_libraries(${_name} err)
  endif()
endmacro()

add_library(err err.c)

add_executable(echo-server echo-server.c)
add_executable(echo-client echo-client.c)

add_executable(nums-server nums-server.c)
add_executable(nums-client nums-client.c)

add_executable(struct-in-memory struct-in-memory.c)

install(TARGETS DESTINATION .)
