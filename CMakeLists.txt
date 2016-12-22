cmake_minimum_required(VERSION 3.6)
project(projetTLF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/expression_rationnelle.cpp
        src/expression_rationnelle.hpp
        src/lex.yy.cpp
        src/location.hh
        src/ndet.cpp
        src/parser.hpp
        src/parser.lex
        src/parser.tab.cpp
        src/parser.tab.hpp
        src/parser.y
        src/parser_defs.hpp
        src/position.hh
        src/stack.hh)

add_executable(projetTLF ${SOURCE_FILES})