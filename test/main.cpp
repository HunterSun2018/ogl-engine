#include <future>
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
    using namespace glm;
    size_t width = 1600, height = 900;

    //
    //  Instance the engine and set window properties
    //
    auto engine = Engine::instance();
    engine->create_window(width, height, "Demo");

    auto program_factory = engine->get_program_factory();
    auto list = program_factory->list();

    //
    //  Create a sence and add some meshes
    //
    auto scene = make_shared<Scene>();
    auto material = make_shared<Material>(32.f);
    auto mesh = Mesh::create_from_grid(100, 100, program_factory->get_program("Phong"));
    mesh->set_texture(TEXTURE_TYPE::TEX_DIFFUSE,
                      Texture::create_2D_texture("resources/textures/2d/floor.jpg", "floor", false));
    mesh->set_texture(TEXTURE_TYPE::TEX_SPECULAR,
                      Texture::create_2D_texture("resources/textures/2d/floor_specular.png", "floor_specular", false));
    mesh->set_material(material);

    scene->add(mesh);

    scene->set_direction_light(glm::vec3{-1.f, -1.f, -1.f},
                               glm::vec3{0.2f, 0.2f, 0.2f},
                               glm::vec3{1.0f, 1.0f, 1.0f},
                               glm::vec3{1.0f, 1.0f, 1.0f});

    PointLight red_point_light = {.position = vec3(10.f, 3.0f, -20.f),
                                  .enable = true,
                                  .constant = 1.0f,
                                  .linear = 0.09,
                                  .quadratic = 0.032,
                                  .ambient = vec3(0.5f, 0.05f, 0.05f),
                                  .diffuse = vec3(0.8f, 0.0f, 0.0f),
                                  .specular = vec3(1.0f, 0.0f, 0.0f)};

    PointLight green_point_light = {.position = vec3(-10.f, 3.0f, -20.f),
                                    .enable = true,
                                    .constant = 1.0f,
                                    .linear = 0.09,
                                    .quadratic = 0.032,
                                    .ambient = vec3(0.05f, 0.5f, 0.05f),
                                    .diffuse = vec3(0.0f, 0.8f, 0.0f),
                                    .specular = vec3(0.0f, 1.0f, 0.0f)};

    scene->add_point_light(make_shared<PointLight>(red_point_light));
    scene->add_point_light(make_shared<PointLight>(green_point_light));

    auto morak = Model::create_from_file("resources/models/morak/morak.fbx", program_factory);
    // auto task_morak = std::async([=]() -> auto { //
    //     // return Model::create_from_file("resources/models/morak/morak.fbx", program_factory);
    //     return nullptr;
    // });
    // auto jennifer = async(&Model::create_from_file, "resources/models/jennifer/jennifer.fbx", program_factory);

    auto mat_model = glm::translate(mat4(1.0f), vec3(3.f, 0.f, -5.0f));
    mat_model = glm::scale(mat_model, vec3(0.01, 0.01, 0.01));

    scene->add(morak, mat_model);

    auto jennifer = Model::create_from_file("resources/models/jennifer/jennifer.fbx", program_factory);

    mat_model = glm::translate(mat4(1.0f), vec3(-3.f, 0.f, -5.0f));
    mat_model = glm::scale(mat_model, vec3(0.01, 0.01, 0.01));

    scene->add(jennifer, mat_model);

    //
    //  Create a perspetive camera, set position and look at
    //
    auto camera = Camera::create_from_perspective(45.0f, double(width) / height, 0.1, 1000);
    camera->set_position(0, 2, 2);
    camera->look_at(0, 2, 0);

    //
    //  Render sence with camera
    //
    engine->render(scene, camera);
}