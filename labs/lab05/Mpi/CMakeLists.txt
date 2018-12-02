cmake_minimum_required(VERSION 3.10)
project(mpis_stuff)

set(CMAKE_CXX_STANDARD 11)
include_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Include\\x64")
link_libraries("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64\\msmpi.lib")

add_executable(mpis_stuff main.cpp)