#include "pch.hpp"
#include "material.hpp"

using namespace std;

namespace ogle
{
    Material::Material(float shininess)
    {
        _shininess = shininess;
    }

    Material::~Material()
    {
    }
}