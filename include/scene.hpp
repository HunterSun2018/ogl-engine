#pragma once
#include "mesh.hpp"
#include "light.hpp"
#include "program.hpp"
#include "model.hpp"

namespace ogle
{

    class Scene
    {
    private:
        /* data */
        std::list<std::tuple<mesh_ptr, glm::mat4>> _meshes;
        std::list<std::tuple<model_ptr,glm::mat4>> _models;
        DirectionLight _dir_light;
        
        std::vector<point_light_ptr> _point_lights;

        void update_lights(program_light_ptr program_light);

    public:
        Scene(/* args */);
        ~Scene();

        void add(mesh_ptr mesh, glm::mat4 mat = glm::mat4(1.0f));

        void add(model_ptr model, glm::mat4 mat = glm::mat4(1.0f));

        void set_direction_light(const glm::vec3 &dir,
                                 const glm::vec3 &ambient,
                                 const glm::vec3 &diffuse,
                                 const glm::vec3 &specular);

        void add_point_light(point_light_ptr point_light);

        void draw(program_ptr program);
    };

    using sence_ptr = std::shared_ptr<Scene>;

}