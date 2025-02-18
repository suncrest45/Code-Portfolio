cmake_minimum_required(VERSION 3.27)
project(OAHashTable)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(OAHashTable
        driver.cpp
        OAHashTable.cpp
        OAHashTable.h
        Support.cpp
        Support.h)
