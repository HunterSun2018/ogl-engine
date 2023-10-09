/**
 * @file mesh.hpp
 * @author Hunter Sun (HunterSun2018@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
//#include "pch.hpp"
#include <functional>
#include "texture.hpp"
#include "program.hpp"

#define ALIGNED16 __attribute__((aligned(16)))

namespace ogle
{
    enum VertexAttr
    {
        VA_POSTION,
        VA_NORMAL,
        VA_TEXCOORD,
        VA_BONES,
        VA_WEIGHT,
    };

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    enum TEXTURE_TYPE
    {
        TEX_DIFFUSE,
        TEX_SPECULAR,
        TEX_NORMAL,
        TEX_SHININESS,
    };

    class Mesh
    {
        program_ptr _program;
        material_ptr _material;
        std::map<TEXTURE_TYPE, texture_ptr> _textures;
        uint _indieces_count = 0;

    public:
        virtual ~Mesh();

        static std::shared_ptr<Mesh>
        create(std::vector<Vertex> &&vertices,
               std::vector<GLuint> &&indices,
               std::map<TEXTURE_TYPE, texture_ptr> &&textures, program_ptr program);

        virtual void draw(double time, bool wired = false,
                          std::function<void(program_ptr)> set_custom_state = nullptr);

        // void set_diffuse_texture(texture_ptr tex) { _tex_diffuse = tex; };
        // void set_specular_texture(texture_ptr tex) { _tex_sepcular = tex; };

        void set_texture(TEXTURE_TYPE type, texture_ptr texture);

        void set_material(material_ptr material);

        program_ptr get_program() { return _program; };

        //
        //  Create a grid
        //  x : from -width/2 to width/2
        //  y : 0
        //  z : from hight/2 to -hight/2
        //
        static std::shared_ptr<Mesh> create_from_grid(size_t width, size_t hight, program_ptr phong_program);

    private:
        GLuint VAO = 0, VBO = 0, EBO = 0;

        void setup(std::vector<Vertex> &&vertices,
                   std::vector<GLuint> &&indices);
    };

    using mesh_ptr = std::shared_ptr<Mesh>;

    ///////////////////////////////////////////////
    //  SkinnedMesh
    ///////////////////////////////////////////////
    const size_t MAX_BONE_AMOUNT_PER_VERTEX = 4;
    struct SkinnedVertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        GLuint Bones[MAX_BONE_AMOUNT_PER_VERTEX];
        GLfloat Weights[MAX_BONE_AMOUNT_PER_VERTEX];
    };

    struct Bone
    {
        std::string name;
        glm::mat4 offset_matrix;
        glm::mat4 final_transformation = glm::mat4(1.0f);
        std::vector<size_t> children_index;
    };

    class SkinnedMesh : public Mesh
    {
        skinned_program_ptr _program;
        GLuint VAO = 0, VBO = 0, EBO = 0;
        size_t _index_size = 0;

        std::vector<Bone> _bones;

        std::map<TEXTURE_TYPE, texture_ptr> _textures;

        void setup();

    public:
        SkinnedMesh(std::vector<SkinnedVertex> &&vertices,
                    std::vector<GLuint> &&indices,
                    std::map<TEXTURE_TYPE, texture_ptr> &&textures,
                    std::vector<Bone> &&bones,
                    skinned_program_ptr skinned_program);

        void update_bone_transformation(std::string_view bone_name, const glm::mat4 &transformation);

        virtual void draw(double time,
                          bool wired = false,
                          std::function<void(program_ptr)> set_custom_state = nullptr);

    };

    using skinned_mesh_ptr = std::shared_ptr<SkinnedMesh>;
}