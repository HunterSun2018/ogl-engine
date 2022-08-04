#include "pch.hpp"
#include "mesh.hpp"
//#include <ranges>

using namespace std;

namespace ogle
{
    Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, bool wired)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->_wired = wired;

        this->setupMesh();
    }

    Mesh::~Mesh()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void Mesh::draw()
    {
        if (_wired) // Wireframe Mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (_texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _texture->getTexID());
        }

        glBindVertexArray(this->VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        if (_wired) // Wireframe Mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Mesh::setupMesh()
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
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

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
    std::shared_ptr<Mesh> Mesh::create_from_grid(size_t width, size_t hight, bool wired)
    {
        vector<Vertex> vertices;
        vector<GLuint> indices;

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

        return make_shared<Mesh>(vertices, indices, wired);
    }
}