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
#include "texture.hpp"
#include "program.hpp"

namespace ogle
{
    enum
    {
        VATTR_VERTEX,
        VATTR_NORMAL,
        VATTR_TEXCOORD
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
    public:
        std::vector<Vertex> _vertices;
        std::vector<GLuint> indices;

        material_ptr _material;
        // texture_ptr _tex_diffuse;
        // texture_ptr _tex_sepcular;
        std::map<TEXTURE_TYPE, texture_ptr> _textures;

        Mesh(std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices, std::map<TEXTURE_TYPE, texture_ptr> &&textures);
        ~Mesh();

        void draw(program_ptr program, bool wired = false);

        // void set_diffuse_texture(texture_ptr tex) { _tex_diffuse = tex; };
        // void set_specular_texture(texture_ptr tex) { _tex_sepcular = tex; };

        void set_texture(TEXTURE_TYPE type, texture_ptr texture);

        void set_material(material_ptr material);
        //
        //  Create a grid
        //  x : from -width/2 to width/2
        //  y : 0
        //  z : from hight/2 to -hight/2
        //
        static std::shared_ptr<Mesh> create_from_grid(size_t width, size_t hight);

    private:
        GLuint VAO = 0, VBO = 0, EBO = 0;

        void setup();
    };

    using mesh_ptr = std::shared_ptr<Mesh>;
}