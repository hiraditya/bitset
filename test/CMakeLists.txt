cmake_minimum_required(VERSION 2.8)
set(test
  test1.cpp
  )
# The executables to be generated
add_executable(test ${test})
# The libraries to link against
#target_link_libraries(test test)
