cmake_minimum_required(VERSION 3.6)
project(projetTLF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(projetTLF COMMAND make -C ${projetTLF_SOURCE_DIR}
                            CLION_EXE_DIR=${PROJET_BINARY_DIR})

#set(SOURCE_FILES src/ndet.cpp)
#add_executable(projetTLF ${SOURCE_FILES})