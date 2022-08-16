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
    using direction_light_ptr = std::shared_ptr<DirectionLight>;

    struct PointLight
    {
        glm::vec3 ALIGNED16 position;

        bool enable = false;
        float constant;
        float linear;
        float quadratic;

        glm::vec3 ALIGNED16 ambient;
        glm::vec3 ALIGNED16 diffuse;
        glm::vec3 ALIGNED16 specular;
    };
    using point_light_ptr = std::shared_ptr<PointLight>;

    const size_t MAX_POINT_LIGHT_NUMBER = 4;
} // namespace ogle
