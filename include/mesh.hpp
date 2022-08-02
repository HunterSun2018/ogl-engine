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

    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        texture_ptr _texture = nullptr;
        bool _wired = false;

        Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, bool wired = false);
        ~Mesh();

        void draw();

        void set_texture(texture_ptr tex) { _texture = tex; };
        //
        //  Create a grid
        //  x : from -width/2 to width/2
        //  y : 0
        //  z : from hight/2 to -hight/2
        //
        static std::shared_ptr<Mesh> create_from_grid(size_t width, size_t hight, bool wired);

    private:
        GLuint VAO = 0, VBO = 0, EBO = 0;

        void setupMesh();
    };

    using mesh_ptr = std::shared_ptr<Mesh>;
}