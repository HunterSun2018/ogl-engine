#pragma once

#include "pch.hpp"

namespace ogle
{
    struct Skybox
    {
        static std::shared_ptr<Skybox>
        create(std::string_view cube_textures);

        virtual ~Skybox() = default;

        virtual void draw() = 0;
    };

    using skybox_ptr = std::shared_ptr<Skybox>;
}
