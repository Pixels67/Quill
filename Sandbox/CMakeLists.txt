cmake_minimum_required(VERSION 3.15)
project(QuillSandbox)

set(CMAKE_CXX_STANDARD 20)

add_executable(QuillSandbox main.cpp)

target_link_libraries(QuillSandbox Quill)