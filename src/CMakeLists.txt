include_directories(../include 
                /usr/include/freetype2 
                /usr/include/libpng16
                /usr/include/imgui)

aux_source_directory(. SRC)

#add_library(ogle SHARED glad.c engine.cpp camera.cpp material.cpp mesh.cpp model.cpp sence.cpp)
add_library(ogle SHARED ${SRC})

target_link_libraries(ogle GL glfw dl fmt stb assimp imgui)

install(TARGETS ogle)


