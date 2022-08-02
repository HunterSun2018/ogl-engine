#pragma once
#include "pch.hpp"

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

        void draw();
    };

    using model_ptr = std::shared_ptr<Model>;
}
