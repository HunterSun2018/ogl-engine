#include "pch.hpp"
#include "sence.hpp"

namespace ogle
{

    Sence::Sence(/* args */)
    {
    }

    Sence::~Sence()
    {
    }

    void Sence::add(mesh_ptr mesh)
    {
        _meshes.emplace_back(mesh);
    }

    void Sence::add(model_ptr model)
    {
        _models.emplace_back(model);
    }

    void Sence::set_direction_light(const glm::vec3 &dir,
                                    const glm::vec3 &ambient,
                                    const glm::vec3 &diffuse,
                                    const glm::vec3 &specular)
    {
        _dir_light.direction = dir;
        _dir_light.ambient = ambient;
        _dir_light.diffuse = diffuse;
        _dir_light.specular = specular;
    }

    void Sence::update_lights(program_light_ptr program_light)
    {
        if (program_light)
        {
            program_light->set_direction_light(_dir_light);

            for (int i = 0; i < MAX_POINT_LIGHT_NUMBER; i++)
                program_light->set_point_light(i, _point_light[i]);
        }
    }

    void Sence::draw(program_ptr program)
    {
        for (auto mesh : _meshes)
        {
            mesh->draw(program);
        }

        for (auto model : _models)
        {
            model->draw(program);
        }
    }

}