cmake_minimum_required(VERSION 3.10)
project(CellautEngine)


set(RESSOURCES_PATH /Users/nathan/Programmation/ressources)
set(CMAKE_MODULE_PATH ${RESSOURCES_PATH}/CMake)

find_package(GAPI)

include_directories(${GAPI_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)

add_executable(CellautEngine main.cpp CanonicalTree.cpp CanonicalTree.hpp Node.cpp Node.hpp CellautTree.cpp CellautTree.hpp)
target_link_libraries(CellautEngine ${GAPI_LIBRARY})