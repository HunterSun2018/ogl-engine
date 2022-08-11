#pragma once
#include "pch.hpp"
#include "program.hpp"

namespace ogle
{
    class Camera
    {
    private:
        /* data */
        glm::mat4 _project;
        glm::vec3 _pos;
        glm::vec3 _lookat;

    public:
        Camera(glm::mat4 mat);
        ~Camera();

        static std::shared_ptr<Camera>
        create_from_perspective(float degrees, float aspect, float near, float far);

        static std::shared_ptr<Camera>
        create_from_ortho(float left, float right, float bottom, float top, float near, float far);

        void set_position(float x, float y, float z);
        auto get_position() { return _pos; };

        void look_at(float x, float y, float z);

        void process_input(GLFWwindow* window);
        
        glm::mat4 get_project_matrix();

        glm::mat4 get_view_matrix();

        void update_program_matrix(program_matrix_ptr program);
    };

    using camera_ptr = std::shared_ptr<Camera>;
}