#include "pch.hpp"
#include "program.hpp"

using namespace std;
using namespace fmt;
using namespace glm;

namespace ogle
{
    GLuint load_shader(std::string_view file_name, int type)
    {
        ifstream ifs(file_name.data(), ios::in | ios::binary);
        if (!ifs.is_open())
            throw runtime_error(format("failed to load shader file {}", file_name));

        GLuint shader = glCreateShader(type);

        if (string(file_name).substr(file_name.length() - 4, 4) == ".spv")
        {
            vector<uint8_t> buffer(istreambuf_iterator<char>(ifs), {});
            glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, buffer.data(), buffer.size());
            glSpecializeShader(shader, "main", 0, 0, 0);
        }
        else
        {
            string buffer(istreambuf_iterator<char>(ifs), {});
            char *txt[] = {buffer.data()};

            glShaderSource(shader, 1, txt, 0);
            glCompileShader(shader);
        }

        //
        //  Check the status for the compiled shader
        //
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            GLint len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            string log(len, 0);
            glGetShaderInfoLog(shader, len, 0, (GLchar *)log.data());

            glDeleteShader(shader);

            throw runtime_error(format("faild to compile shader : {}, info : {}", file_name, log));
        }

        return shader;
    }

    GLuint create_program(std::string_view vs_file_name, std::string_view fs_file_name)
    {
        GLuint vs = load_shader(vs_file_name, GL_VERTEX_SHADER);
        GLuint fs = load_shader(fs_file_name, GL_FRAGMENT_SHADER);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);

        //
        //  Link program
        //
        glLinkProgram(prog);

        GLint status;
        glGetProgramiv(prog, GL_LINK_STATUS, &status);
        if (!status)
        {
            GLint len;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);

            string log(len, 0);
            glGetProgramInfoLog(prog, len, 0, (char *)log.data());

            glDeleteShader(vs);
            glDeleteShader(fs);
            glDeleteProgram(prog);

            throw runtime_error(format("failed to link shader program : {}", log));
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        return prog;
    }

    class SimpleProgramImp : public SimpleProgram
    {
        GLuint _program;
        GLuint _ubo_vs;
        glm::mat4 _mat_model, _mat_view, _mat_project;

        struct Uniform
        {
            glm::mat4 mat_mv;
            glm::mat4 mat_mvp;
        } _uniform;

    public:
        SimpleProgramImp(GLuint program)
        {
            _program = program;

            glGenBuffers(1, &_ubo_vs);
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Uniform), nullptr, GL_STREAM_DRAW);
        }

        virtual void set_model_matrix(const glm::mat4 &model) override
        {
            _mat_model = model;
        }

        virtual void set_view_matrix(const glm::mat4 &view) override
        {
            _mat_view = view;
        }

        virtual void set_project_matrix(const glm::mat4 &project) override
        {
            _mat_project = project;
        }

        virtual void set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p) override
        {
            _mat_model = m;
            _mat_view = v;
            _mat_project = p;
        }

        virtual void apply() override
        {
            _uniform.mat_mvp = _mat_project * _mat_view * _mat_model;
            _uniform.mat_mv = _mat_view * _mat_model;

            glUseProgram(_program);

            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Uniform), &_uniform);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo_vs);
        }
    };

    std::shared_ptr<SimpleProgram>
    SimpleProgram::create(std::string_view vs_file_name, std::string_view fs_file_name)
    {
        auto program = make_shared<SimpleProgramImp>(create_program(vs_file_name, fs_file_name));

        return static_pointer_cast<SimpleProgram>(program);
    }

    /**
     * @brief Phong light model program
     *
     */
    class PhongProgramImp : public PhongProgram
    {
    private:
        /* data */
        enum uniform_binding_index
        {
            VERTEX,
            MATERIAL,
            LIGHTING,
        };

        GLuint _program;
        GLuint _ubo_vs, _ubo_material, _ubo_fs;

        glm::mat4 _mat_model, _mat_view, _mat_project;
        material_ptr _material;
        DirectionLight _dir_light;
        PointLight _point_lights[4];

        struct Uniform
        {
            glm::mat4 mat_view;
            glm::mat4 mat_project;
            glm::mat4 mat_mv;
            glm::mat4 mat_mvp;
            glm::vec3 v3_light_pos;
        } _uniform; //__attribute__((packed))

        struct FsUniform // Fragment shader uniform
        {
            DirectionLight dir_light;
            PointLight point_lights[MAX_POINT_LIGHT_NUMBER];
        } _fs_uniform; //__attribute__((packed))

    public:
        PhongProgramImp(GLuint prog)
        {
            _program = prog;

            glUseProgram(_program);

            glGenBuffers(1, &_ubo_vs);
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Uniform), &_uniform, GL_STREAM_DRAW);

            glGenBuffers(1, &_ubo_material);
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_material);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), nullptr, GL_STREAM_DRAW);

            glGenBuffers(1, &_ubo_fs);
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_fs);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(FsUniform), nullptr, GL_STREAM_DRAW);
        }

        ~PhongProgramImp()
        {
            if (glIsProgram(_program))
                glDeleteProgram(_program);
        }

        void set_light_pos(glm::vec3 pos);

    public:
        //
        //  Program methods
        //
        void apply() override
        {
            _fs_uniform.dir_light.direction = glm::mat3(_mat_view) * (_dir_light.direction);

            for (size_t i = 0; i < 4; i++)
                _fs_uniform.point_lights[i].position = _mat_view * vec4(_point_lights[i].position, 1.0);

            _uniform.mat_view = _mat_view;
            _uniform.mat_project = _mat_project;
            _uniform.mat_mvp = _mat_project * _mat_view * _mat_model;
            _uniform.mat_mv = _mat_view * _mat_model;

            glUseProgram(_program);

            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Uniform), &_uniform);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo_vs);

            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_material);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), _material.get());
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, _ubo_material);

            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_fs);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FsUniform), &_fs_uniform);
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _ubo_fs);
        }
        //
        //  IProgramMaterial interfaces
        //
        virtual void set_material(material_ptr materail) override
        {
            _material = materail;
        }
        //
        //  IProgramMatrix interfaces
        //
        virtual void set_model_matrix(const glm::mat4 &model) override
        {
            _mat_model = model;
        }

        virtual void set_view_matrix(const glm::mat4 &view) override
        {
            _mat_view = view;
        }

        virtual void set_project_matrix(const glm::mat4 &project) override
        {
            _mat_project = project;
        }

        void set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p) override
        {
            _mat_model = m;
            _mat_view = v;
            _mat_project = p;
        }
        //
        //  IProgramLight interfaces
        //
        virtual void set_direction_light(const DirectionLight &dir_light) override
        {
            _dir_light = dir_light;
            _fs_uniform.dir_light = _dir_light;
        }

        virtual void set_point_light(size_t index, const PointLight &point_light) override
        {
            _point_lights[index] = point_light;
            _fs_uniform.point_lights[index] = point_light;
        }
    };

    shared_ptr<PhongProgram>
    PhongProgram::create(std::string_view vs_file_name, std::string_view fs_file_name)
    {
        return static_pointer_cast<PhongProgram>(make_shared<PhongProgramImp>(create_program(vs_file_name, fs_file_name)));
    }

    /**
     * @brief Skinned mesh program implementation
     *
     */
    class SkinnedProgramImp : public SkinnedProgram
    {
        enum uniform_binding_index
        {
            VERTEX,
            MATERIAL,
            LIGHTING,
        };

        GLuint _program;
        GLuint _ubo_vs, _ubo_fs, _ubo_bones;

        glm::mat4 _mat_model, _mat_view, _mat_project;
        material_ptr _material;
        DirectionLight _dir_light;
        PointLight _point_lights[4];

        struct VsUniform
        {
            glm::mat4 bones[100];
            glm::mat4 mat_mv;
            glm::mat4 mat_mvp;
            // std::array<glm::mat4, 100> bones;
        } _vs_uniform;

        // static const int MAX_BONES = 100;
        // struct VsUniform
        // {
        //     glm::mat4 bones[MAX_BONES];
        //     glm::mat4 model;
        //     glm::mat4 view;
        //     glm::mat4 projection;
        // } _vs_uniform;

        struct BoneTransforamtion
        {
            glm::mat4 bones[100];
        } _bone_transformation;

        struct FsUniform // Fragment shader uniform
        {
            DirectionLight dir_light;
            PointLight point_lights[MAX_POINT_LIGHT_NUMBER];
        } _fs_uniform; //__attribute__((packed))

    public:
        SkinnedProgramImp(GLuint program)
        {
            _program = program;

            glUseProgram(_program);

            glGenBuffers(1, &_ubo_vs);
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(VsUniform), nullptr, GL_STREAM_DRAW);

            // glGenBuffers(1, &_ubo_bones);
            // glBindBuffer(GL_UNIFORM_BUFFER, _ubo_bones);
            // glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), glm::_bone_transformation.bones[0], GL_STREAM_DRAW);

            glGenBuffers(1, &_ubo_fs);
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_fs);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(FsUniform), nullptr, GL_STREAM_DRAW);
        }

        ~SkinnedProgramImp()
        {
            if (glIsProgram(_program))
                glDeleteProgram(_program);
        }

        virtual void set_material(material_ptr material) override
        {
            _material = material;
        }

        virtual void set_model_matrix(const glm::mat4 &model) override
        {
            _mat_model = model;
        }

        virtual void set_view_matrix(const glm::mat4 &view) override
        {
            _mat_view = view;
        }

        virtual void set_project_matrix(const glm::mat4 &project) override
        {
            _mat_project = project;
        }

        virtual void set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p) override
        {
            _mat_view = v;
            _mat_project = p;
        }

        virtual void set_direction_light(const DirectionLight &dir_light) override
        {
            _dir_light = dir_light;
        }

        virtual void set_point_light(size_t index, const PointLight &point_light) override
        {
            if (index >= MAX_POINT_LIGHT_NUMBER)
                throw range_error(format("The index {} is more than MAX_POINT_LIGHT_NUMBER {}", index, MAX_POINT_LIGHT_NUMBER));

            _point_lights[index] = point_light;
        }

        virtual void set_bone_transforms(const std::array<glm::mat4, 100> &transforms) override
        {
            //_vs_uniform.bones = transforms;
            copy(begin(transforms), end(transforms), begin(_vs_uniform.bones));
        }

        virtual void apply() override
        {
            _fs_uniform.dir_light.direction = glm::mat3(_mat_view) * (_dir_light.direction);

            for (size_t i = 0; i < 4; i++)
                _fs_uniform.point_lights[i].position = _mat_view * vec4(_point_lights[i].position, 1.0);

            _vs_uniform.mat_mvp = _mat_project * _mat_view * _mat_model;
            _vs_uniform.mat_mv = _mat_view * _mat_model;
            // _vs_uniform.projection = _mat_project;
            // _vs_uniform.view = _mat_view;
            // _vs_uniform.model = _mat_model;

            glUseProgram(_program);

            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(VsUniform), &_vs_uniform);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo_vs);

            // glBindBuffer(GL_UNIFORM_BUFFER, _ubo_fs);
            // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FsUniform), &_fs_uniform);
            // glBindBufferBase(GL_UNIFORM_BUFFER, 2, _ubo_fs);
        }
    };

    std::shared_ptr<SkinnedProgram>
    SkinnedProgram::create(std::string_view vs_file_name, std::string_view fs_file_name)
    {
        return static_pointer_cast<SkinnedProgram>(
            make_shared<SkinnedProgramImp>(create_program(vs_file_name, fs_file_name)));
    }
}