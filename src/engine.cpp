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

    /**
     * @brief
     *
     * @param error
     * @param description
     */
    static void glfw_error_callback(int error, const char *description)
    {
        throw runtime_error(format("glfw error: {}", description));
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

#ifndef NDEBUG
        //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

        return shared_ptr<Engine>(new Engine());
    }

    /**
     * @brief
     *
     * @param window
     * @param width
     * @param height
     */
    void glfw_window_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    /**
     * @brief
     *
     * @param window
     * @param key
     * @param scancode
     * @param action
     * @param mode
     */
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    /**
     * @brief A callback function for capturing mouse event
     *
     * @param window
     * @param xpos
     * @param ypos
     */
    void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
    {
        // Engine::instance()->cursor_pos_event(window, xpos, ypos);
    }

    void Engine::set_window(size_t width, size_t height, std::string_view title)
    {
        if (window)
            glfwDestroyWindow(window);

        window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

        glfwSetKeyCallback(window, key_callback);

        glfwSetCursorPosCallback(window, cursor_pos_callback);

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
            glfwPollEvents();

            camera->process_input(window);

            //
            //  Clear the screen
            //
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 model(1.0f); // identity
            glm::mat4 project = camera->get_project_matrix();
            glm::mat4 view = camera->get_view_matrix();

            program->set_mvp_matrices(model, view, project);
            auto light_pos = view * glm::vec4(0.f , 10.f, -10.f, 1.0f);
            program->set_light_pos(light_pos);
            program->apply();

            sence->draw();

            auto error = glGetError();
            check_error(error);

            // showFPS(window);

            glfwSwapBuffers(window);
        }
    }

    void Engine::cursor_pos_event(GLFWwindow *window, double xpos, double ypos)
    {
    }

}