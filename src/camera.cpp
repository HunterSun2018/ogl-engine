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
        _lookat.y = z;
    }

    glm::mat4 Camera::get_project_matrix()
    {
        return _project;
    }

    glm::mat4 Camera::get_view_matrix()
    {
        return glm::lookAt(_pos, _lookat, glm::vec3(0.f, 1.f, 0.0f));   // up is positive y
    }

}