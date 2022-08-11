#pragma once
#include "mesh.hpp"
#include "light.hpp"
#include "program.hpp"
#include "model.hpp"

namespace ogle
{

    class Sence
    {
    private:
        /* data */
        std::list<mesh_ptr> _meshes;
        std::list<model_ptr> _models;
        DirectionLight _dir_light;
        PointLight _point_light[MAX_POINT_LIGHT_NUMBER];

    public:
        Sence(/* args */);
        ~Sence();

        void add(mesh_ptr mesh);

        void add(model_ptr model);

        void set_direction_light(const glm::vec3 &dir,
                                 const glm::vec3 &ambient,
                                 const glm::vec3 &diffuse,
                                 const glm::vec3 &specular);

        void update_lights(program_light_ptr program_light);

        void draw(program_ptr program);
    };

    using sence_ptr = std::shared_ptr<Sence>;

}