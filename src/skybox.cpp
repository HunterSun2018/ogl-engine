#include "skybox.hpp"

using namespace std;

namespace ogle
{
    class SkyboxImp : public Skybox
    {
    private:
        GLuint _vao;

    public:
        SkyboxImp(/* args */)
        {
            initialize();
        }

        virtual ~SkyboxImp()
        {
        }

        virtual void draw() override
        {
        }

        void initialize()
        {
            GLfloat skyboxVertices[] = {
                // Positions
                -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f};

            GLuint vbo;

            glGenVertexArrays(1, &_vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
            glBindVertexArray(0);
        }

        // Loads a cubemap texture from 6 individual texture faces
        // Order should be:
        // +X (right)
        // -X (left)
        // +Y (top)
        // -Y (bottom)
        // +Z (front)
        // -Z (back)
        GLuint loadCubemap(vector<const GLchar *> faces)
        {
            GLuint textureID;
            glGenTextures(1, &textureID);

            int width, height;
            unsigned char *image;

            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            for (GLuint i = 0; i < faces.size(); i++)
            {
                // image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
                // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                // SOIL_free_image_data(image);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            return textureID;
        }
    };

    std::shared_ptr<Skybox>
    Skybox::create(std::string_view cube_textures)
    {
        return static_pointer_cast<Skybox>(
            make_shared<SkyboxImp>());
    }
}
