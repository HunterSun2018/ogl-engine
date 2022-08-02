#include "pch.hpp"
#include "utils.hpp"
#include "program.hpp"
#include "engine.hpp"

using namespace std;
using namespace fmt;

namespace ogle
{
    std::shared_ptr<Engine> Engine::_instance = nullptr;

    Engine::Engine(/* args */)
    {
    }

    Engine::~Engine()
    {
        if (window)
            glfwDestroyWindow(window);

        glfwTerminate();
    }

    static void glfw_error_callback(int error, const char *description)
    {
        throw runtime_error(fmt::format("glfw error: {}", description));
    }

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

#define ENUM2STR(x) \
    case (x):       \
        return (#x)

    void GLAPIENTRY gldebug(GLenum src, GLenum type, GLuint id, GLenum severity,
                            GLsizei len, const char *msg, const void *cls)
    {
        auto eunm_to_str = [](GLenum type) -> string
        {
            switch (type)
            {
                // Source
                ENUM2STR(GL_DEBUG_SOURCE_API);
                ENUM2STR(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
                ENUM2STR(GL_DEBUG_SOURCE_SHADER_COMPILER);
                ENUM2STR(GL_DEBUG_SOURCE_THIRD_PARTY);
                ENUM2STR(GL_DEBUG_SOURCE_APPLICATION);
                ENUM2STR(GL_DEBUG_SOURCE_OTHER);
                // Type
                ENUM2STR(GL_DEBUG_TYPE_ERROR);
                ENUM2STR(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
                ENUM2STR(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
                ENUM2STR(GL_DEBUG_TYPE_PORTABILITY);
                ENUM2STR(GL_DEBUG_TYPE_PERFORMANCE);
                ENUM2STR(GL_DEBUG_TYPE_OTHER);
            default:
                return "unkonw";
            }
        };

        cerr << fmt::format("[GLDEBUG] {} {} : {}", eunm_to_str(src), eunm_to_str(type), msg);
    }

    void GLAPIENTRY check_error(GLenum error)
    {
        auto eunm_to_str = [](GLenum type) -> string
        {
            switch (type)
            {

                ENUM2STR(GL_NO_ERROR);
                ENUM2STR(GL_INVALID_ENUM);
                ENUM2STR(GL_INVALID_VALUE);
                ENUM2STR(GL_INVALID_OPERATION);
                ENUM2STR(GL_STACK_OVERFLOW);
                ENUM2STR(GL_STACK_UNDERFLOW);
                ENUM2STR(GL_OUT_OF_MEMORY);
                ENUM2STR(GL_INVALID_FRAMEBUFFER_OPERATION);
                ENUM2STR(GL_CONTEXT_LOST);
            default:
                return "unkonw";
            }
        };

        if (error != GL_NO_ERROR)
        {
            cerr << fmt::format("[glGetError] : {}\n", eunm_to_str(error));
            exit(1);
        }
    }

    std::shared_ptr<Engine> Engine::instance()
    {
        if (_instance)
            return _instance;

        //
        //  Initialize glfw
        //
        glfwSetErrorCallback(glfw_error_callback);

        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_SAMPLES, 4);

        // gladLoadGL();

        // glDebugMessageCallback(gldebug, nullptr);
        // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        return shared_ptr<Engine>(new Engine());
    }

    void glfw_window_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Engine::set_window(size_t width, size_t height, std::string_view title)
    {
        if (window)
            glfwDestroyWindow(window);

        window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

        glfwSetKeyCallback(window, key_callback);

        glfwSetWindowSizeCallback(window, glfw_window_size_callback);

        glfwMakeContextCurrent(window);

        gladLoadGL();

        glfwSwapInterval(1);

        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glDepthFunc(GL_LESS);

        glDebugMessageCallback(gldebug, nullptr);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

    void showFPS(GLFWwindow *pWindow)
    {
        // Measure speed
        static double lastTime = 0;
        static size_t nbFrames = 0;

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        nbFrames++;

        if (delta >= 1.0)
        { // If last cout was more than 1 sec ago
            cout << 1000.0 / double(nbFrames) << endl;

            double fps = double(nbFrames) / delta;

            std::stringstream ss;
            ss << "[" << fps << " FPS]";

            glfwSetWindowTitle(pWindow, ss.str().c_str());

            nbFrames = 0;
            lastTime = currentTime;
        }
    }
    void Engine::render(sence_ptr sence, camera_ptr camera)
    {
        if (!window)
            throw runtime_error("The method 'set_window' wasn't called at once.");

        string shaders_path = "resources/shaders";
        auto program = Program::create_program(shaders_path + "/spirv/vertex.spv",
                                               shaders_path + "/spirv/fragment.spv");
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        while (!glfwWindowShouldClose(window))
        {
            //
            //  Clear the screen
            //
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 model(1.0f); // identity
            glm::mat4 project = camera->get_project_matrix();
            glm::mat4 view = camera->get_view_matrix();

            program->set_mvp_matrices(model, view, project);
            program->set_light_pos({10, 10, 10});
            program->apply();

            sence->draw();

            auto error = glGetError();
            check_error(error);

            //showFPS(window);

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }
}