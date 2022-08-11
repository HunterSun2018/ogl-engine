#pragma once
#include "pch.hpp"

#define ALIGNED16 __attribute__((aligned(16)))

namespace ogle
{
    struct DirectionLight
    {
        glm::vec3 ALIGNED16 direction;
        glm::vec3 ALIGNED16 ambient;
        glm::vec3 ALIGNED16 diffuse;
        glm::vec3 ALIGNED16 specular;
    };

    struct PointLight
    {
        glm::vec3 ALIGNED16 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ALIGNED16 ambient;
        glm::vec3 ALIGNED16 diffuse;
        glm::vec3 ALIGNED16 specular;
    };

    const size_t MAX_POINT_LIGHT_NUMBER = 4;
} // namespace ogle
