#pragma once

#include "program.hpp"
#include "camera.hpp"
#include "scene.hpp"

namespace ogle
{
    class Engine : public ProgramFactory,
                   public std::enable_shared_from_this<Engine>
    {
    private:
        /* data */
        GLFWwindow *window = nullptr;
        static std::shared_ptr<Engine> _instance;

        std::map<std::string, program_ptr> _programs;

        Engine(/* args */);

        void create_programs();

    public:
        ~Engine();

        static std::shared_ptr<Engine> instance();

        void create_window(size_t width, size_t height, std::string_view title);

        void render(sence_ptr sence, camera_ptr camera);

        program_factory_ptr get_program_factory()
        {
            auto engine = shared_from_this();
            
            return static_pointer_cast<ProgramFactory>(engine);            
        }

        virtual std::vector<std::string_view> list_progrom() override
        {
            using namespace std;
            vector<string_view> list;

            transform(begin(_programs), end(_programs), back_inserter(list), [](auto iter)
                      { return iter.first; });

            return list;
        }

        virtual program_ptr get_program(std::string_view program_name) override
        {
            using namespace std;

            if (auto iter = _programs.find(program_name.data()); iter != end(_programs))
                return iter->second;
            else
                return nullptr;
        }

        //
        // Events
        //
        void cursor_pos_event(GLFWwindow *window, double xpos, double ypos);
    };

    using engine_ptr = std::shared_ptr<Engine>;
}