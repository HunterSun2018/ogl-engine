set(CMAKE_EXE_LINKER_FLAGS -Wl,-rpath=./lib)

include_directories(../include)

add_executable(demo main.cpp)
target_link_libraries(demo ogle)

install(TARGETS demo)