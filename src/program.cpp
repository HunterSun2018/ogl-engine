#include "pch.hpp"
#include "program.hpp"

using namespace std;
using namespace fmt;

namespace ogle
{
    Program::Program(GLuint prog)
    {
        _program = prog;

        glUseProgram(_program);

        glGenBuffers(1, &_ubo_vs);
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Uniform), &_uniform, GL_STREAM_DRAW);
    }

    Program::~Program()
    {
        if (glIsProgram(_program))
            glDeleteProgram(_program);
    }

    GLuint Program::load_shader(std::string_view file_name, int type)
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
            glShaderSource(shader, 1, (const GLchar **)buffer.data(), 0);
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

    shared_ptr<Program>
    Program::create_program(std::string_view vs_file_name, std::string_view fs_file_name)
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

        return make_shared<Program>(prog);
    }

    void Program::initialize()
    {
        //glUseProgram(_program);

        glGenBuffers(1, &_ubo_vs);
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Uniform), &_uniform, GL_STREAM_DRAW);
    }

    void Program::apply()
    {        
        glUseProgram(_program);

        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_vs);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Uniform), &_uniform);
        
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo_vs);
    }

    void Program::set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p)
    {
        _uniform.mat_view = v;
        _uniform.mat_project = p;
        _uniform.mat_mvp = p * v * m;
        _uniform.mat_mv = v * m;
    }

    void Program::set_light_pos(glm::vec3 pos)
    {
        _uniform.v3_light_pos = pos;
    }

}