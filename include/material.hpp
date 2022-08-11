#pragma once
#include "pch.hpp"

namespace ogle
{
    class Material
    {
    private:
        /* data */
        float _shininess;
    public:
        Material(float shininess);
        ~Material();
    };

    using material_ptr = std::shared_ptr<Material>;    
}