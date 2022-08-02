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

    void Sence::draw()
    {
        for (auto mesh : _meshes)
        {
            mesh->draw();
        }

        for (auto model : _models)
        {
            model->draw();
        }
    }

}