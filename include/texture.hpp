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

        static std::shared_ptr<Texture>
        generate_2D_texture(u_char *pdata, GLuint width, GLuint height, GLuint numComponents, std::string_view name);

    public:
        Texture();
        ~Texture();

        //
        //  Create texture from file
        //
        static std::shared_ptr<Texture>
        create_2D_texture(std::string_view texPath, std::string_view texName, bool texFlip);
        //
        //  Create texture from memory
        //
        static std::shared_ptr<Texture>
        create_2D_texture(u_char *pdata, size_t size, std::string_view name, bool flip);
        
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

    struct CubeTexture
    {        
        virtual ~CubeTexture() = default;

        
    };
    
    // CubeTexture::CubeTexture(/* args */)
    // {
    // }
    
    // CubeTexture::~CubeTexture()
    // {
    // }
    
}