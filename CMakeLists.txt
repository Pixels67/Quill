cmake_minimum_required(VERSION 3.14)
project(Quill)

set(CMAKE_CXX_STANDARD 20)

add_library(
        Quill STATIC
        Quill/src/Token.hpp
        Quill/src/Lexer.cpp
        Quill/src/Lexer.hpp
        Quill/include/QuillValue.hpp
        Quill/src/Parser.cpp
        Quill/src/Parser.hpp
        Quill/include/Quill.hpp
        Quill/src/Quill.cpp
)

target_include_directories(Quill PUBLIC Quill/src Quill/include)

add_subdirectory(Sandbox)