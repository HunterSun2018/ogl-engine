#pragma once
#include "pch.hpp"
#include "material.hpp"
#include "program.hpp"

namespace ogle
{
    class Model
    {
    private:
        /* data */        

    public:
        Model(/* args */);
        ~Model();

        void load(std::string_view model_file_name, std::string_view tex_file_name);
        

        void draw(program_ptr program);
    };

    using model_ptr = std::shared_ptr<Model>;
}
