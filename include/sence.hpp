#pragma once
#include "mesh.hpp"
#include "model.hpp"

namespace ogle
{

    class Sence
    {
    private:
        /* data */
        std::list<mesh_ptr> _meshes;
        std::list<model_ptr> _models;

    public:
        Sence(/* args */);
        ~Sence();

        void add(mesh_ptr mesh);

        void add(model_ptr model);

        void draw();
    };

    using sence_ptr = std::shared_ptr<Sence>;

}