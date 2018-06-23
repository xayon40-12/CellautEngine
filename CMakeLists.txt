cmake_minimum_required(VERSION 3.10)
project(CellautEngine)


set(RESSOURCES_PATH /Users/nathan/Programmation/ressources)
set(CMAKE_MODULE_PATH ${RESSOURCES_PATH}/CMake)

find_package(GAPI)

include_directories(${GAPI_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)
configure_file(kernel.cl kernel.cl COPYONLY)
configure_file(screen.vert screen.vert COPYONLY)
configure_file(screen.frag screen.frag COPYONLY)

add_executable(CellautEngine main.cpp CanonicalTree.cpp CanonicalTree.hpp Node.cpp Node.hpp CellautTree.cpp CellautTree.hpp DrawableCanonicalTree.cpp DrawableCanonicalTree.hpp)
target_link_libraries(CellautEngine ${GAPI_LIBRARY})