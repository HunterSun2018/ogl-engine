cmake_minimum_required(VERSION 3.16)
project(test)

set(CMAKE_CXX_COPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS -Werror)

#target_precompile_headers(mytest PUBLIC include/pch.hpp)

add_subdirectory(src)

add_subdirectory(test)

# install(DIRECTORY resources DESTINATION .)