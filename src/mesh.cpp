#include "pch.hpp"
#include "mesh.hpp"
//#include <ranges>

using namespace std;
using namespace glm;

namespace ogle
{
    std::shared_ptr<Mesh>
    Mesh::create(std::vector<Vertex> &&vertices,
                 std::vector<GLuint> &&indices,
                 std::map<TEXTURE_TYPE, texture_ptr> &&textures,
                 program_ptr program)
    {
        auto mesh = make_shared<Mesh>();

        mesh->_textures = move(textures);
        mesh->_program = program;
        mesh->_indieces_count = indices.size();

        mesh->setup(move(vertices), move(indices));

        return mesh;
    }

    Mesh::~Mesh()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void Mesh::draw(double time, bool wired,
                    std::function<void(program_ptr)> set_custom_state)
    {
        auto program_material = dynamic_pointer_cast<ProgramMaterial>(_program);
        if (program_material && _material)
        {
            program_material->set_material(_material);
        }

        if (wired) // Wireframe Mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (set_custom_state)
            set_custom_state(_program);

        _program->apply();

        if (_textures.find(TEX_DIFFUSE) != end(_textures))
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _textures[TEX_DIFFUSE]->getTexID());
        }

        if (_textures.find(TEX_SPECULAR) != end(_textures))
        {
            glActiveTexture(GL_TEXTURE0 + TEX_SPECULAR);
            glBindTexture(GL_TEXTURE_2D, _textures[TEX_SPECULAR]->getTexID());
        }

        glBindVertexArray(this->VAO);

        glDrawElements(GL_TRIANGLES, this->_indieces_count, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        if (wired) // Wireframe Mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Mesh::set_texture(TEXTURE_TYPE type, texture_ptr texture)
    {
        _textures[type] = texture;
    }

    void Mesh::set_material(material_ptr material)
    {
        _material = material;
    }

    void Mesh::setup(std::vector<Vertex> &&vertices,
                     std::vector<GLuint> &&indices)
    {
        //
        // A VAO(vertext array object) object contains a VBO(array buffer object) object and a EBO(element buffer object)
        //
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        //
        //  Bind VAO
        //
        glBindVertexArray(this->VAO);

        //
        //  Bind VBO and EBO for VAO
        //
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        //
        //  Set vertex attribute points
        //
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, TexCoords));

        //
        //  Unbind VAO object
        //
        glBindVertexArray(0);
    }
    /**
     * @brief Create a gird with width and hight
     *
     * @param width
     * @param hight
     * @param wired
     * @return std::shared_ptr<Mesh>
     * Description : reate a grid
     *  x : from -width/2 to width/2
     *  y : 0
     *  z : from hight/2 to -hight/2
     */
    std::shared_ptr<Mesh> Mesh::create_from_grid(size_t width, size_t hight, program_ptr phong_program)
    {
        vector<Vertex> vertices;
        vector<GLuint> indices;
        map<TEXTURE_TYPE, texture_ptr> textures;

        //
        //  Create vertices
        //
        for (size_t j = 0; j <= hight; j++)
        {
            for (size_t i = 0; i <= width; i++)
            {
                vertices.push_back(Vertex{
                    glm::vec3(-float(width) / 2 + i,
                              0.f,
                              float(hight) / 2 - j),
                    {0.f, 1.f, 0.f}, // normal
                    {i % 2 ? 1.f : 0.f, j % 2 ? 1.0f : 0.f}});
            }
        }
        //
        //  Create indices
        //
        for (size_t j = 0; j < hight; j++)
        {
            for (size_t i = 0; i < width; i++)
            {
                indices.push_back(j * (width + 1) + i);
                indices.push_back((j + 1) * (width + 1) + i + 1);
                indices.push_back((j + 1) * (width + 1) + i);

                indices.push_back(j * (width + 1) + i);
                indices.push_back(j * (width + 1) + i + 1);
                indices.push_back((j + 1) * (width + 1) + i + 1);
            }
        }

        return Mesh::create(move(vertices), move(indices), move(textures), phong_program);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  SkinnedMesh
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SkinnedMesh::SkinnedMesh(std::vector<SkinnedVertex> &&vertices,
                             std::vector<GLuint> &&indices,
                             std::map<TEXTURE_TYPE, texture_ptr> &&textures,
                             std::vector<Bone> &&bones,
                             skinned_program_ptr skinned_program)
    {
        _textures = move(textures);
        _bones = move(bones);
        _program = skinned_program;

        //
        // A VAO(vertext array object) object contains a VBO(array buffer object) object and a EBO(element buffer object)
        //
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        //
        //  Bind VAO
        //
        glBindVertexArray(this->VAO);

        //
        //  Bind VBO and EBO for VAO
        //
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        _index_size = indices.size();

        //
        //  Set vertex attribute points
        //
        glEnableVertexAttribArray(VA_POSTION);
        glVertexAttribPointer(VA_POSTION, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid *)0);

        glEnableVertexAttribArray(VA_NORMAL);
        glVertexAttribPointer(VA_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid *)offsetof(SkinnedVertex, Normal));

        glEnableVertexAttribArray(VA_TEXCOORD);
        glVertexAttribPointer(VA_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid *)offsetof(SkinnedVertex, TexCoords));

        glEnableVertexAttribArray(VA_BONES);
        glVertexAttribIPointer(VA_BONES, 4, GL_INT, sizeof(SkinnedVertex), (GLvoid *)offsetof(SkinnedVertex, Bones));

        glEnableVertexAttribArray(VA_WEIGHT);
        glVertexAttribPointer(VA_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid *)offsetof(SkinnedVertex, Weights));

        if (glGetError() != GL_NO_ERROR)
            throw runtime_error("Failed to generate buffer!");
        //
        //  Unbind VAO object
        //
        glBindVertexArray(0);
    }

    void SkinnedMesh::update_bone_transformation(std::string_view bone_name, const mat4 &transformation)
    {
        auto compare_name = [=](const auto &bone) -> bool
        {
            return bone.name == bone_name;
        };

        if (auto bone = find_if(begin(_bones), end(_bones), compare_name); bone != end(_bones))
        {
            bone->final_transformation = transformation * bone->offset_matrix;
        }
    }

    void SkinnedMesh::draw(double time, bool wired,
                           std::function<void(program_ptr)> set_custom_state)
    {
        auto program_material = dynamic_pointer_cast<ProgramMaterial>(_program);
        if (program_material)
        {
            // program_material->set_material(_material);
        }

        if (auto program_skinned = dynamic_pointer_cast<ProgramSkinned>(_program))
        {
            array<mat4, 100> bone_transformation;

            // For debug skinned vertex shader
            fill(begin(bone_transformation), end(bone_transformation), mat4(1.0));

            size_t i = 0;

            for (auto bone : _bones)
            {
                bone_transformation[i++] = bone.final_transformation; // mat4(1.0); //
            }

            program_skinned->set_bone_transforms(bone_transformation);
        };

        if (set_custom_state)
            set_custom_state(_program);

        _program->apply();

        if (_textures.find(TEX_DIFFUSE) != end(_textures))
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _textures[TEX_DIFFUSE]->getTexID());
        }

        if (_textures.find(TEX_SPECULAR) != end(_textures))
        {
            glActiveTexture(GL_TEXTURE0 + TEX_SPECULAR);
            glBindTexture(GL_TEXTURE_2D, _textures[TEX_SPECULAR]->getTexID());
        }

        if (wired) // Wireframe Mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, this->_index_size, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        if (wired) // Wireframe Mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}