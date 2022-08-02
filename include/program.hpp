#pragma once

namespace ogle
{
    class Program
    {
    private:
        /* data */
        GLuint _program;
        GLuint _ubo_vs;
        static GLuint load_shader(std::string_view file_name, int type);

        struct Uniform
        {
            glm::mat4   mat_view;
            glm::mat4   mat_project;
            glm::mat4   mat_mv;
            glm::mat4   mat_mvp;
            glm::vec3   v3_light_pos;
        }  _uniform;    //__attribute__((packed))



    public:
        Program(GLuint prog);
        ~Program();

        static std::shared_ptr<Program>
        create_program(std::string_view vs_file_name, std::string_view fs_file_name);

        void initialize();
        
        void apply();

        void set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p);
        void set_light_pos(glm::vec3 pos);
    };

    using shader_ptr = std::shared_ptr<Program>;
}