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
        create_from_file(std::string_view model_file_name, program_factory_ptr program_factory);

        virtual void draw(double time, bool wired = false,
                          std::function<void(program_ptr)> set_custom_state = nullptr) = 0;
    };

    using model_ptr = std::shared_ptr<Model>;
}
