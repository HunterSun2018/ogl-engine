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
    engine->create_window(width, height, "Demo");

    //
    //  Create a sence and add some meshes
    //
    auto sence = make_shared<Sence>();
    auto material = make_shared<Material>(32.f);
    auto mesh = Mesh::create_from_grid(100, 100, false);
    mesh->set_diffuse_texture(Texture::create_2D_texture("resources/textures/2d/floor.jpg", "floor", false));
    mesh->set_specular_texture(Texture::create_2D_texture("resources/textures/2d/floor_specular.png", "floor_specular", false));
    mesh->set_material(material);

    sence->add(mesh);

    sence->set_direction_light(glm::vec3{-1.f, -1.f, 0.f},
                               glm::vec3{0.2f, 0.2f, 0.2f},
                               glm::vec3{0.5f, 0.5f, 0.5f},
                               glm::vec3{1.0f, 1.0f, 1.0f});

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