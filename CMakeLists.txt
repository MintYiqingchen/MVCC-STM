cmake_minimum_required(VERSION 2.8)
PROJECT(stm)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/include)
ADD_SUBDIRECTORY(lib)
add_executable(stm1 smt1.cpp)
add_executable(stm2 smt2.cpp)
add_executable(stm3 smt3.cpp)
TARGET_LINK_LIBRARIES(stm1 pthread STMLibs)
TARGET_LINK_LIBRARIES(stm2 pthread STMLibs)
TARGET_LINK_LIBRARIES(stm3 pthread STMLibs)
