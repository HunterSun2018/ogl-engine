include_directories(../include)

aux_source_directory(. SRC)

#add_library(ogle SHARED glad.c engine.cpp camera.cpp material.cpp mesh.cpp model.cpp sence.cpp)
add_library(ogle SHARED ${SRC})

target_link_libraries(ogle GL GLU glfw dl fmt stb)

install(TARGETS ogle)
