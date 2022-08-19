#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "model.hpp"

using namespace std;
using namespace fmt;
using namespace glm;

namespace ogle
{
    class ModelImp : public Model
    {
        std::vector<mesh_ptr> _meshes;
        std::string _path;
        const aiScene *scene;

    public:
        ModelImp(std::string_view model_file_name)
        {
            Assimp::Importer importer;
            scene = importer.ReadFile(model_file_name.data(), aiProcess_Triangulate | aiProcess_FlipUVs);

            if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                throw runtime_error(format("ERROR::ASSIMP:: {}", importer.GetErrorString()));
            }

            string path(model_file_name);
            _path = path.substr(0, path.find_last_of("/"));

            this->processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode *node, const aiScene *scene)
        {
            // 添加当前节点中的所有Mesh
            for (GLuint i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                this->_meshes.push_back(this->processMesh(mesh, scene));
            }
            // 递归处理该节点的子孙节点
            for (GLuint i = 0; i < node->mNumChildren; i++)
            {
                this->processNode(node->mChildren[i], scene);
            }
        }

        mesh_ptr processMesh(aiMesh *mesh, const aiScene *scene)
        {
            vector<Vertex> vertices;
            vector<GLuint> indices;
            map<TEXTURE_TYPE, texture_ptr> textures;

            //处理顶点
            for (GLuint i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex{vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
                              vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
                              vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)};
                // mesh->mTextureCoords[0] ? vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : vec2(0.f, 0.f)};

                // 处理顶点坐标、法线和纹理坐标
                vertices.emplace_back(vertex);
            }
            // 处理顶点索引
            for (GLuint i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (GLuint j = 0; j < face.mNumIndices; j++)
                    indices.emplace_back(face.mIndices[j]);
            }
            // 处理材质
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

                // Diffuse
                auto tex_diffuse = this->load_material_texture(material,
                                                               aiTextureType_DIFFUSE, "texture_diffuse");
                textures[TEX_DIFFUSE] = tex_diffuse;

                // Specular
                auto tex_specular = this->load_material_texture(material,
                                                                aiTextureType_SPECULAR, "texture_diffuse");
                textures[TEX_SPECULAR] = tex_specular;

                // Normal
                auto tex_normals = this->load_material_texture(material,
                                                               aiTextureType_NORMALS, "texture_diffuse");
                textures[TEX_NORMAL] = tex_normals;

                // glossiness
                auto tex_shininess = this->load_material_texture(material,
                                                                 aiTextureType_SHININESS, "texture_diffuse");
                textures[TEX_SHININESS] = tex_shininess;
            }

            return make_shared<Mesh>(move(vertices), move(indices), move(textures));
        }

        texture_ptr load_material_texture(aiMaterial *mat, aiTextureType type, string typeName)
        {
            auto count = mat->GetTextureCount(type);
            for (GLuint i = 0; i < count; i++)
            {
                aiString tex_path;
                mat->GetTexture(type, i, &tex_path);

                // auto texture = Texture::create_2D_texture(_path + str.C_Str(), str.C_Str(), false);
                auto ai_text = scene->GetEmbeddedTexture(tex_path.C_Str());

                auto width = ai_text->mWidth;
                auto hight = ai_text->mHeight;
                auto component = ai_text->achFormatHint;
                auto data = ai_text->pcData;

                if (ai_text && hight == 0)
                {
                    // if aiTexture::mHeight == 0 is fullfilled. Then, texture is stored in a "compressed" format such as DDS or PNG.
                    // aiTexture::mWidth specifies the size of the texture data in bytes,
                    // aiTexture::pcData is a pointer to the raw image data
                    // aiTexture::achFormatHint is either zeroed or contains the most common file extension of the embedded texture's format
                    auto texture = Texture::create_2D_texture((u_char *)data, width, tex_path.C_Str(), false);

                    return texture;
                }
            }

            return nullptr;
        }

        void draw(program_ptr program, bool wired) override
        {
            for (auto &mesh : _meshes)
            {
                mesh->draw(program);
            }
        }
    };

    std::shared_ptr<Model>
    Model::create_from_file(std::string_view model_file_name)
    {
        auto model = make_shared<ModelImp>(model_file_name);

        return dynamic_pointer_cast<Model>(model);
    }
}