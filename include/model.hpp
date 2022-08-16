#pragma once
#include "pch.hpp"
#include "material.hpp"
#include "program.hpp"
#include "mesh.hpp"

namespace ogle
{
    class Model
    {
    public:
        ~Model() = default;

        static std::shared_ptr<Model>
        create_from_file(std::string_view model_file_name);

        virtual void draw(program_ptr program, bool wired = false) = 0;
    };

    using model_ptr = std::shared_ptr<Model>;
}
