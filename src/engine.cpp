#include "pch.hpp"
#include "utils.hpp"
#include "program.hpp"
//   -I/usr/include/imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
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
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        _programs.clear();

        if (window)
            glfwDestroyWindow(window);

        glfwTerminate();
    }

    void init_imgui(GLFWwindow *window, const char *glsl_version)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        //(void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        // ImGui::StyleColorsDark();
        ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        // io.Fonts->AddFontDefault();
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        // IM_ASSERT(font != NULL);
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

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        glfwWindowHint(GLFW_SAMPLES, 4);

#ifndef NDEBUG
        // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
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

    void Engine::create_window(size_t width, size_t height, std::string_view title)
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

        init_imgui(window, "#version 450");

        create_programs();

        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glDepthFunc(GL_LESS);

        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DEBUG_SOURCE_API,
                              GL_DEBUG_TYPE_ERROR,
                              GL_DEBUG_SEVERITY_HIGH,
                              0, nullptr, GL_TRUE);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

    void Engine::create_programs()
    {
        string shaders_path = "resources/shaders";

        auto program = PhongProgram::create(shaders_path + "/spirv/vertex.spv",
                                            shaders_path + "/spirv/fragment.spv");
        _programs["Phong"] = program;
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
            // cout << ss.str();

            nbFrames = 0;
            lastTime = currentTime;
        }
    }

    void show_gui(GLFWwindow *window)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            // Our state
            static bool show_demo_window = true;
            static bool sync_refresh_rate = true;
            static float f = 0.0f;
            static int counter = 0;
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

            ImGui::Begin("OpenGL Demo!"); // Create a window called "Hello, world!" and append into it.
            ImGui::SetWindowFontScale(2.0f);

            ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            ImGui::Text("Resolution : width = %d, height = %d", width, height);

            ImGui::Checkbox("Sync refresh rate", &sync_refresh_rate);
            ImGui::SameLine();
            ImGui::Text(" : %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            sync_refresh_rate ? glfwSwapInterval(1) : glfwSwapInterval(0);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
    }

    void Engine::render(sence_ptr sence, camera_ptr camera)
    {
        if (!window)
            throw runtime_error("The method 'set_window' wasn't called at once.");

        auto program = _programs["Phong"];

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            glfwGetTime();

            camera->process_input(window);
            show_gui(window);

            //
            //  Clear the screen
            //
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update program matrices
            camera->update_program_matrix(dynamic_pointer_cast<ProgramMatrix>(program));

            sence->draw(program);

            auto error = glGetError();
            check_error(error);

            // showFPS(window);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

    void Engine::cursor_pos_event(GLFWwindow *window, double xpos, double ypos)
    {
    }

}