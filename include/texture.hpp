#pragma once

#include "pch.hpp"

namespace ogle
{

    class Texture
    {

        GLuint texID, texWidth, texHeight, texComponents;
        GLfloat anisoFilterLevel;
        GLenum texType, texInternalFormat, texFormat;
        std::string texName;

    public:
        Texture();
        ~Texture();

        static std::shared_ptr<Texture>
        create_2D_texture(const char *texPath, std::string texName, bool texFlip);

        void setTextureHDR(const char *texPath, std::string texName, bool texFlip);
        void setTextureHDR(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type, GLenum minFilter);
        void setTextureCube(std::vector<const char *> &faces, bool texFlip);
        void setTextureCube(GLuint width, GLenum format, GLenum internalFormat, GLenum type, GLenum minFilter);
        void computeTexMipmap();
        GLuint getTexID();
        GLuint getTexWidth();
        GLuint getTexHeight();
        std::string getTexName();
        void useTexture();
    };

    using texture_ptr = std::shared_ptr<Texture>;
}