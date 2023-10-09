echo "$0 $1"

# complie phong shaders
glslangValidator phong.vert -V -o spirv/vertex.spv $1
glslangValidator phong.frag -V -o spirv/fragment.spv $1

# compile skinned shaders
glslangValidator skinned.vert -V -o spirv/vs_skinned.spv $1
glslangValidator skinned.frag -V -o spirv/fs_skinned.spv $1

# compile simple shaders
glslangValidator simple.vert -V -o spirv/vs_simple.spv $1
glslangValidator simple.frag -V -o spirv/fs_simple.spv $1

# compile skybox shaders
glslangValidator skybox.vert -V -o spirv/vs_skybox.spv $1
glslangValidator skybox.frag -V -o spirv/fs_skybox.spv $1