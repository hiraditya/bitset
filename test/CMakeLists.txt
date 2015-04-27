# Bitset implementation.
#
#  Copyright [2015-] Aditya Kumar.
#  This file is distributed under the GPL v3 License. See LICENSE for details.
#

cmake_minimum_required(VERSION 2.8)
set(test
  #test1.cpp
  BitVectorTest.cpp
  )

include_directories(/home/hiraditya/work/googletest-read-only/include)

set(gtest /home/hiraditya/work/googletest-read-only/build/libgtest.a)
set(gtest_main /home/hiraditya/work/googletest-read-only/build/libgtest_main.a)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGTEST_NO_LLVM_RAW_OSTREAM")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_NO_LLVM_RAW_OSTREAM")

# The executables to be generated
add_executable(test ${test})
# The libraries to link against
target_link_libraries(test ${gtest} ${gtest_main} pthread)
