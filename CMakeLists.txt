cmake_minimum_required(VERSION 3.0.0)
set(This tic-tac-toe)
project(${This} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(headers
  Solver.hpp 
  ElementPool.hpp 
  Minimax.hpp 
  MCTS.hpp 
  Board.hpp
)

set(sources
  main.cpp
  Minimax.cpp
  Board.cpp
  MCTS.cpp
)

add_executable(${This} ${headers} ${sources})

target_compile_options(${This} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wall -Werror -Wextra>>
  $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wall -Werror -Wextra>>
  $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:MSVC>:/Wall>>
)