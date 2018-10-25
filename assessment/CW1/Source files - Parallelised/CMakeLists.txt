cmake_minimum_required(VERSION 3.10)
project(block_chain_par)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

add_executable(block_chain_par main.cpp block_chain.cpp block_chain.h block_chain_par_while.cpp block_chain_par_while.h sha256.cpp sha256.h sha256_par_for.cpp sha256_par_for.h)