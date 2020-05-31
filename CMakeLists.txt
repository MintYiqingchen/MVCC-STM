cmake_minimum_required(VERSION 2.8)
PROJECT(stm)

SET(CMAKE_CXX_STANDARD 11)
aux_source_directory(. DIR_SRCS)

ADD_EXECUTABLE(stm ${DIR_SRCS})
