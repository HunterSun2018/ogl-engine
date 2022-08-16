#include "pch.hpp"
#include "scene.hpp"

using namespace std;

namespace ogle
{

    Scene::Scene(/* args */)
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::add(mesh_ptr mesh, glm::mat4 mat)
    {
        _meshes.emplace_back(make_tuple<>(mesh, mat));
    }

    void Scene::add(model_ptr model, glm::mat4 mat)
    {
        _models.emplace_back(make_tuple<>(model, mat));
    }

    void Scene::set_direction_light(const glm::vec3 &dir,
                                    const glm::vec3 &ambient,
                                    const glm::vec3 &diffuse,
                                    const glm::vec3 &specular)
    {
        _dir_light.direction = dir;
        _dir_light.ambient = ambient;
        _dir_light.diffuse = diffuse;
        _dir_light.specular = specular;
    }

    void Scene::add_point_light(point_light_ptr point_light)
    {
        _point_lights.emplace_back(point_light);
    }

    void Scene::update_lights(program_light_ptr program_light)
    {
        if (program_light)
        {
            program_light->set_direction_light(_dir_light);

            for (int i = 0; i < _point_lights.size() && MAX_POINT_LIGHT_NUMBER; i++)
                program_light->set_point_light(i, *_point_lights[i]);
        }
    }

    void update_model_matrix(program_matrix_ptr program_matrix, const glm::mat4 &model_matrix)
    {
        if (program_matrix)
        {
            program_matrix->set_model_matrix(model_matrix);
        }
    }

    void Scene::draw(program_ptr program)
    {
        update_lights(dynamic_pointer_cast<ProgramLight>(program));

        for (const auto &[mesh, mat] : _meshes)
        {
            update_model_matrix(dynamic_pointer_cast<ProgramMatrix>(program), mat);

            mesh->draw(program);
        }

        for (const auto &[model, mat] : _models)
        {
            update_model_matrix(dynamic_pointer_cast<ProgramMatrix>(program), mat);

            model->draw(program);
        }
    }

}