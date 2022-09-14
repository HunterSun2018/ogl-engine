#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <cassert>
#include "model.hpp"

using namespace std;
using namespace fmt;
using namespace glm;

namespace ogle
{
    // For converting between ASSIMP and glm
    static inline glm::vec3 vec3_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
    static inline glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
    static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
    static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
    static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

    struct KeyFrame
    {
        float time;
        vec3 scaling;
        quat rotation;
        vec3 translation;
    };

    struct Animation
    {
        double ticks_per_second;
        double duration;
        std::map<string, vector<KeyFrame>> node_anims;
    };
    using animation_ptr = std::shared_ptr<Animation>;

    struct Node
    {
        string name;
        mat4 transformation;
        vector<shared_ptr<Node>> children;
    };
    using node_ptr = std::shared_ptr<Node>;

    class ModelImp : public Model
    {
        std::vector<mesh_ptr> _meshes;
        std::vector<animation_ptr> _animations;
        std::string _path;
        node_ptr _root_node;

        program_factory_ptr _program_factory;

        struct BoneVertex
        {
            vec3 pos;
            vec3 color;
        };
        std::vector<BoneVertex> _bone_vertices;
        GLuint _vao_bones;
        size_t _bone_line_size = 0;
        ;

    public:
        ModelImp(std::string_view model_file_name, program_factory_ptr program_factory)
        {
            cout << format("Starting to load model file '{}' ...", model_file_name) << endl;

            _program_factory = program_factory;

            Assimp::Importer importer;

            importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

            const aiScene *scene = importer.ReadFile(model_file_name.data(),
                                                     aiProcessPreset_TargetRealtime_MaxQuality |
                                                         aiProcess_Triangulate |
                                                         aiProcess_FlipUVs);

            if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                throw runtime_error(format("ERROR::ASSIMP:: {}", importer.GetErrorString()));
            }

            string path(model_file_name);
            _path = path.substr(0, path.find_last_of("/"));

            _root_node = this->load_node(scene->mRootNode, scene);

            for (size_t i = 0; i < scene->mNumMeshes; i++)
            {
                auto mesh = load_mesh(scene->mMeshes[i], scene);

                this->_meshes.push_back(mesh);
            }

            for (uint i = 0; i < scene->mNumAnimations; i++)
            {
                auto anim = load_animation(scene->mAnimations[i]);
                _animations.push_back(move(anim));
            }

            _bone_line_size = generate_bone_vbo();

            cout << format("ending to load model file '{}' ...", model_file_name) << endl;
        }

        node_ptr load_node(aiNode *pNode, const aiScene *scene)
        {
            auto node = make_shared<Node>();

            node->name = pNode->mName.C_Str();
            node->transformation = mat4_cast(pNode->mTransformation);

            // 添加当前节点中的所有Mesh
            // for (GLuint i = 0; i < pNode->mNumMeshes; i++)
            // {
            //     aiMesh *mesh = scene->mMeshes[pNode->mMeshes[i]];
            //     this->_meshes.push_back(this->load_mesh(mesh, scene));
            // }

            // traverse all children nodes
            for (GLuint i = 0; i < pNode->mNumChildren; i++)
            {
                auto child = this->load_node(pNode->mChildren[i], scene);
                node->children.push_back(child);
            }

            return node;
        }

        size_t generate_bone_vbo()
        {
            BoneVertex lines[] = {
                {{0.f, 0.0f, -5.0f}, // point 0
                 {1.0f, 0.f, 1.f}},  // color 0
                {{0.f, 2.0f, -5.0f}, // point 1
                 {0.0f, 1.f, 1.f}},  // color 1
            };
            vector<BoneVertex> bone_vetices;
            vec3 color1 = {1.0f, 0.f, 1.f}, color2 = {0.0f, 1.f, 1.f};

            function<void(node_ptr, vec3)> generate_bone_line = [&](node_ptr parent_node, vec3 parent_point)
            {
                // if (parent_node->name == "mixamorig:LeftShoulder")
                //     return;

                for (auto child : parent_node->children)
                {
                    vec3 child_pos = vec3(child->transformation * vec4(parent_point, 1.0f));
                    // if (parent_point != vec3(0))
                    {
                        bone_vetices.emplace_back(BoneVertex({parent_point, color1}));
                        bone_vetices.emplace_back(BoneVertex({child_pos, color1}));
                    }

                    generate_bone_line(child, child_pos);
                }
            };

            generate_bone_line(_root_node, vec3(0, 0, 0));

            glGenVertexArrays(1, &_vao_bones);
            glBindVertexArray(_vao_bones);
            {
                GLuint _vbo_bones;

                glGenBuffers(1, &_vbo_bones);
                glBindBuffer(GL_ARRAY_BUFFER, _vbo_bones);
                glBufferData(GL_ARRAY_BUFFER, sizeof(BoneVertex) * bone_vetices.size(), bone_vetices.data(), GL_STATIC_DRAW);

                // location 0
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (GLvoid *)0);
                glEnableVertexAttribArray(0);

                // location 1
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (GLvoid *)offsetof(BoneVertex, color));
                glEnableVertexAttribArray(1);
            }
            // Unbind VAO
            glBindVertexArray(0);

            return bone_vetices.size();
        }
        mesh_ptr load_mesh(aiMesh *pMesh, const aiScene *pScene)
        {
            vector<Vertex> vertices;
            vector<SkinnedVertex> skinned_vertices;
            vector<GLuint> indices;
            map<TEXTURE_TYPE, texture_ptr> textures;
            vector<Bone> bones;

            if (pMesh->mNumBones)
            {
                skinned_vertices.reserve(pMesh->mNumVertices);
                for (GLuint i = 0; i < pMesh->mNumVertices; i++)
                {
                    SkinnedVertex vertex{vec3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z),
                                         vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z),
                                         vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y),
                                         {0, 0, 0, 0},
                                         {0, 0, 0, 0}};

                    skinned_vertices.emplace_back(move(vertex));
                }

                // Update bone info for skinned vertices
                bones = load_bones(pMesh, skinned_vertices);
            }
            else
            {
                //处理顶点
                vertices.reserve(pMesh->mNumVertices);
                for (GLuint i = 0; i < pMesh->mNumVertices; i++)
                {
                    Vertex vertex{vec3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z),
                                  vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z),
                                  vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y)};
                    // mesh->mTextureCoords[0] ? vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : vec2(0.f, 0.f)};

                    // 处理顶点坐标、法线和纹理坐标
                    vertices.emplace_back(vertex);
                }
            }

            // 处理顶点索引
            indices.reserve(pMesh->mNumFaces * 3);
            for (GLuint i = 0; i < pMesh->mNumFaces; i++)
            {
                aiFace face = pMesh->mFaces[i];
                for (GLuint j = 0; j < face.mNumIndices; j++)
                    indices.emplace_back(face.mIndices[j]);
            }

            // 处理材质
            if (pMesh->mMaterialIndex >= 0)
            {
                aiMaterial *material = pScene->mMaterials[pMesh->mMaterialIndex];

                // Diffuse
                auto tex_diffuse = this->load_material_texture(pScene, material,
                                                               aiTextureType_DIFFUSE, "texture_diffuse");
                textures[TEX_DIFFUSE] = tex_diffuse;

                // Specular
                auto tex_specular = this->load_material_texture(pScene, material,
                                                                aiTextureType_SPECULAR, "texture_diffuse");
                textures[TEX_SPECULAR] = tex_specular;

                // Normal
                auto tex_normals = this->load_material_texture(pScene, material,
                                                               aiTextureType_NORMALS, "texture_diffuse");
                textures[TEX_NORMAL] = tex_normals;

                // glossiness
                auto tex_shininess = this->load_material_texture(pScene, material,
                                                                 aiTextureType_SHININESS, "texture_diffuse");
                textures[TEX_SHININESS] = tex_shininess;
            }

            if (pMesh->mNumBones > 0)
            {
                auto skinned_program = dynamic_pointer_cast<SkinnedProgram>(_program_factory->get_program("Skinned"));

                auto skinned_mesh = make_shared<SkinnedMesh>(move(skinned_vertices), move(indices), move(textures), move(bones), skinned_program);
                return dynamic_pointer_cast<Mesh>(skinned_mesh);
            }
            else
            {
                auto program = dynamic_pointer_cast<PhongProgram>(_program_factory->get_program("Phong"));
                return Mesh::create(move(vertices), move(indices), move(textures), program);
            }
        }

        texture_ptr load_material_texture(const aiScene *pScene,
                                          aiMaterial *pMaterial,
                                          aiTextureType type,
                                          string typeName)
        {
            auto count = pMaterial->GetTextureCount(type);
            for (GLuint i = 0; i < count; i++)
            {
                aiString tex_path;
                pMaterial->GetTexture(type, i, &tex_path);

                // auto texture = Texture::create_2D_texture(_path + str.C_Str(), str.C_Str(), false);
                auto ai_text = pScene->GetEmbeddedTexture(tex_path.C_Str());

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

        vector<Bone> load_bones(const aiMesh *pMesh, vector<SkinnedVertex> &skinned_vextices)
        {
            vector<Bone> bones(pMesh->mNumBones);
            vector<uint> vertex_bone_indexes(skinned_vextices.size(), 0);

            for (uint i = 0; i < pMesh->mNumBones; i++)
            {
                auto ai_bone = pMesh->mBones[i];

                bones[i].name = ai_bone->mName.C_Str();
                bones[i].offset_matrix = mat4_cast(ai_bone->mOffsetMatrix);

                for_each_n(ai_bone->mWeights, ai_bone->mNumWeights,
                           [&](aiVertexWeight vertex_weight)
                           {
                               uint vertex_id = vertex_weight.mVertexId;

                               auto &vertex_bone_index = vertex_bone_indexes[vertex_id];

                               if (vertex_bone_index >= MAX_BONE_AMOUNT_PER_VERTEX)
                                   return;
                               assert(vertex_bone_index < MAX_BONE_AMOUNT_PER_VERTEX);

                               skinned_vextices[vertex_id].Bones[vertex_bone_index] = i;
                               skinned_vextices[vertex_id].Weights[vertex_bone_index] = vertex_weight.mWeight;

                               vertex_bone_index++;
                           });

                // for (uint j = 0; j < ai_bone->mNumWeights; j++)
                // {
                //     uint vertex_id = ai_bone->mWeights[j].mVertexId;
                //     float weight = ai_bone->mWeights[j].mWeight;

                //     auto &vertex_bone_index = vertex_bone_indexes[vertex_id];

                //     // Keep amount BONES_AMOUNT bones that has maximus weight
                //     // if (vertex_bone_index >= MAX_BONE_AMOUNT_PER_VERTEX)
                //     // {
                //     //     auto e = min_element(skinned_vextices[vertex_id].Weights, skinned_vextices[vertex_id].Weights + 3);
                //     //     if (weight > *e)
                //     //     {
                //     //         // update the minimum element with the current weight
                //     //         auto dist = distance(skinned_vextices[vertex_id].Weights, e);
                //     //         skinned_vextices[vertex_id].Bones[dist] = i;
                //     //         *e = weight;
                //     //     }
                //     //     continue;
                //     // }

                //     assert(vertex_bone_index < MAX_BONE_AMOUNT_PER_VERTEX);

                //     skinned_vextices[vertex_id].Bones[vertex_bone_index] = i;
                //     skinned_vextices[vertex_id].Weights[vertex_bone_index] = weight;

                //     vertex_bone_index++;
                // }
            }            

            return bones;
        }

        animation_ptr load_animation(const aiAnimation *pAnim)
        {
            auto animation = make_shared<Animation>();

            animation->ticks_per_second = pAnim->mTicksPerSecond != 0 ? pAnim->mTicksPerSecond : 25.0f;
            animation->duration = pAnim->mDuration;

            for (uint i = 0; i < pAnim->mNumChannels; i++)
            {
                const aiNodeAnim *pNodeAnim = pAnim->mChannels[i];

                string name = pNodeAnim->mNodeName.C_Str();

                assert(pNodeAnim->mNumScalingKeys == pNodeAnim->mNumRotationKeys);
                // &&       pNodeAnim->mNumScalingKeys == pNodeAnim->mNumPositionKeys);

                vector<KeyFrame> key_frames(pNodeAnim->mNumScalingKeys);

                for (uint j = 0; j < pNodeAnim->mNumScalingKeys; j++)
                {
                    key_frames[j].time = pNodeAnim->mScalingKeys[j].mTime;

                    key_frames[j].scaling = vec3_cast(pNodeAnim->mScalingKeys[j].mValue);
                    key_frames[j].rotation = quat_cast(pNodeAnim->mRotationKeys[j].mValue.Normalize());
                    key_frames[j].translation = pNodeAnim->mNumPositionKeys == 1 ? vec3_cast(pNodeAnim->mPositionKeys[0].mValue) : vec3_cast(pNodeAnim->mPositionKeys[j].mValue);
                }

                animation->node_anims[name] = move(key_frames);
            }

            return animation;
        }

        void update_node_hierarchy(double anim_time, animation_ptr anim, node_ptr node, const mat4 &parent_transform)
        {
            mat4 node_transformation = node->transformation;

            if (auto iter = anim->node_anims.find(node->name); iter != end(anim->node_anims))
            {
                auto &transform = iter->second[0];

                if (iter->second.size() != 1)
                    transform = iter->second[anim_time];

                mat4 identity(1.0f), scaling, rotation, translation;

                scaling = scale(identity, transform.scaling);
                rotation = glm::mat4_cast(transform.rotation); // glm::normalize(transform.rotation)
                translation = translate(identity, transform.translation);

                node_transformation = translation * rotation * scaling;
            }

            mat4 global_transfrom = parent_transform * node_transformation;

            for (auto mesh : _meshes)
            {
                if (auto skinned_mesh = dynamic_pointer_cast<SkinnedMesh>(mesh))
                    skinned_mesh->update_bone_transformation(node->name, global_transfrom);
            }

            for (auto child : node->children)
            {
                update_node_hierarchy(anim_time, anim, child, global_transfrom);
            }
        }

        void update_bone_animation(float time)
        {
            for (auto anim : _animations)
            {
                mat4 identity(1.0f);
                double anim_time = fmod(time * anim->ticks_per_second, anim->duration);

                update_node_hierarchy(anim_time, anim, _root_node, identity);
            }
        }

        void draw(double time, bool wired,
                  std::function<void(program_ptr)> set_custom_state) override
        {
            update_bone_animation(time);

            for (auto &mesh : _meshes)
            {
                mesh->draw(time, wired, set_custom_state);
            }

            // draw_bones(time, set_custom_state);
        }

        void draw_bones(double time, std::function<void(program_ptr)> set_custom_state)
        {
            static size_t count = 2;
            static bool init = false;
            static double start = 0;

            if (!init)
            {
                start = time;
                init = true;
            }

            auto program = _program_factory->get_program("Simple");
            if (auto simple = dynamic_pointer_cast<SimpleProgram>(program))
            {
                if (set_custom_state)
                    set_custom_state(program);

                simple->apply();
                glDisable(GL_DEPTH_TEST);

                glBindVertexArray(_vao_bones);

                glLineWidth(5);
                glDrawArrays(GL_LINES, 0, count);

                glBindVertexArray(0);
                glEnable(GL_DEPTH_TEST);

                if (count < _bone_line_size)
                    count = (time - start) * 10;
            }
        }
    };

    std::shared_ptr<Model>
    Model::create_from_file(std::string_view model_file_name, program_factory_ptr program_factory)
    {
        auto model = make_shared<ModelImp>(model_file_name, program_factory);

        return dynamic_pointer_cast<Model>(model);
    }
}