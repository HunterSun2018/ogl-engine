#pragma once

//#include "ogle.hpp"
#include "camera.hpp"
#include "sence.hpp"

namespace ogle
{
    class Engine
    {
    private:
        /* data */
        GLFWwindow *window = nullptr;
        static std::shared_ptr<Engine> _instance;
        
        Engine(/* args */);
        
        

    public:
        ~Engine();

        static std::shared_ptr<Engine> instance();

        void set_window(size_t width, size_t height, std::string_view title);

        void render(sence_ptr sence, camera_ptr camera);

        //
        // Events
        //
        void cursor_pos_event(GLFWwindow *window, double xpos, double ypos);
    };

    using engine_ptr = std::shared_ptr<Engine>;
}