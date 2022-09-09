# complie phong shaders
glslangValidator phong.vert -V -g -o spirv/vertex.spv
glslangValidator phong.frag -V -g -o spirv/fragment.spv
# compile skinned shaders
glslangValidator skinned.vert -V -g -o spirv/vs_skinned.spv
glslangValidator skinned.frag -V -g -o spirv/fs_skinned.spv
# compile simple shaders
glslangValidator simple.vert -V -g -o spirv/vs_simple.spv
glslangValidator simple.frag -V -g -o spirv/fs_simple.spv