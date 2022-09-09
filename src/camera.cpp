#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"

using namespace std;

namespace ogle
{
    Camera::Camera(glm::mat4 mat)
    {
        _project = mat;
    }

    Camera::~Camera()
    {
    }

    std::shared_ptr<Camera>
    Camera::create_from_perspective(float degrees, float aspect, float near, float far)
    {
        auto perspective = glm::perspective(glm::radians(degrees), aspect, near, far);

        return make_shared<Camera>(perspective);
    }

    std::shared_ptr<Camera>
    Camera::create_from_ortho(float left, float right, float bottom, float top, float near, float far)
    {
        auto ortho = glm::ortho(left, right, bottom, top, near, far);

        return make_shared<Camera>(ortho);
    }

    void Camera::set_position(float x, float y, float z)
    {
        _pos.x = x;
        _pos.y = y;
        _pos.z = z;
    }

    void Camera::look_at(float x, float y, float z)
    {
        _lookat.x = x;
        _lookat.y = y;
        _lookat.z = z;
    }

    glm::mat4 Camera::get_project_matrix()
    {
        return _project;
    }

    glm::mat4 Camera::get_view_matrix()
    {
        return glm::lookAt(_pos, _lookat, glm::vec3(0.f, 1.f, 0.0f)); // up is positive y
    }

    void Camera::process_input(GLFWwindow *window)
    {
        const GLfloat cameraSpeed = 0.1f;
        static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        cameraFront.y = 0;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            _pos += cameraSpeed * cameraFront;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            _pos -= cameraSpeed * cameraFront;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            _pos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            _pos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        // Update pitch and yaw by cursor pos
        {
            static bool firstMouse = true;
            static double lastX = 0.f, lastY = 0.f;
            static GLfloat yaw = 0.f, pitch = 0.f;

            if (firstMouse)
            {
                int width = 0, height = 0;
                glfwGetWindowSize(window, &width, &height);

                lastX = width / 2;
                lastY = height / 2;

                firstMouse = false;
            }

            double xpos = 0, ypos = 0;
            glfwGetCursorPos(window, &xpos, &ypos);

            GLfloat xoffset = xpos - lastX;
            GLfloat yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            GLfloat sensitivity = 0.05;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            // cameraFront = glm::normalize(front);
        }

        _lookat = _pos + glm::vec3(0.0f, 0.0f, -1.0f); // cameraFront;
    }

    void Camera::update_program_matrix(program_matrix_ptr program)
    {
        if (program)
        {
            program->set_view_matrix(get_view_matrix());
            program->set_project_matrix(get_project_matrix());
        }
    }
}