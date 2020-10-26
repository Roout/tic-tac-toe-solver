cmake_minimum_required(VERSION 3.0.0)
project(minimax VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(minimax main.cpp)

target_compile_options(minimax PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wall -Werror>>
  $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wall -Werror>>
  $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:MSVC>:/Wall>>
)