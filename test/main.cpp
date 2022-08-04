#include "ogle.hpp"

using namespace std;

void run();

int main(int argc, char *argv[])
{
    cout << "ogl engine is getting started ..." << endl;

    try
    {
        run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void run()
{
    using namespace ogle;
    size_t width = 1600, height = 900;

    //
    //  Instance the engine and set window properties
    //
    auto engine = Engine::instance();
    engine->set_window(width, height, "Demo");

    //
    //  Create a sence and add some meshes
    //
    auto sence = make_shared<Sence>();
    auto mesh = Mesh::create_from_grid(100, 100, false);
    mesh->set_texture(Texture::create_2D_texture("resources/textures/2d/floor.jpg", "floor", false));
    sence->add(mesh);

    //
    //  Create a perspetive camera, set position and look at
    //
    auto camera = Camera::create_from_perspective(45.0f, double(width) / height, 0.1, 1000);
    camera->set_position(0, 2, 2);
    camera->look_at(0, 2, 0);

    //
    //  Render sence with camera
    //
    engine->render(sence, camera);
}