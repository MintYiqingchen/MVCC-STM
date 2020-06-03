cmake_minimum_required(VERSION 2.8)
PROJECT(stm)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/include)
ADD_SUBDIRECTORY(lib)
add_executable(stm1 main_old.cpp)

TARGET_LINK_LIBRARIES(stm1 pthread STMLibs)
