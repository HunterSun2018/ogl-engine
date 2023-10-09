#pragma once

#include <glm/glm.hpp>
#include "material.hpp"
#include "light.hpp"

namespace ogle
{
    struct Program
    {
        virtual ~Program() = default;

        virtual void apply() = 0;
    };

    using program_ptr = std::shared_ptr<Program>;

    struct ProgramMatrix
    {
        virtual ~ProgramMatrix() = default;

        virtual void set_model_matrix(const glm::mat4 &world) = 0;

        virtual void set_view_matrix(const glm::mat4 &view) = 0;

        virtual void set_project_matrix(const glm::mat4 &project) = 0;

        virtual void set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p) = 0;
    };

    using program_matrix_ptr = std::shared_ptr<ProgramMatrix>;

    struct ProgramMaterial
    {
        virtual ~ProgramMaterial() = default;

        virtual void set_material(material_ptr materail) = 0;
    };

    using program_material_ptr = std::shared_ptr<ProgramMaterial>;

    struct ProgramLight
    {
        virtual ~ProgramLight() = default;

        virtual void set_direction_light(const DirectionLight &dir_light) = 0;

        virtual void set_point_light(size_t index, const PointLight &point_light) = 0;
    };

    using program_light_ptr = std::shared_ptr<ProgramLight>;

    struct ProgramSkinned
    {
        virtual ~ProgramSkinned() = default;

        virtual void set_bone_transforms(const std::array<glm::mat4, 100> &transforms) = 0;
    };

    using program_skinned_ptr = std::shared_ptr<ProgramSkinned>;

    class SimpleProgram : public Program, public ProgramMatrix
    {
    public:
        virtual ~SimpleProgram() = default;

        static std::shared_ptr<SimpleProgram>
        create(std::string_view vs_file_name, std::string_view fs_file_name);
    };

    using simple_progrm_ptr = std::shared_ptr<SimpleProgram>;

    /**
     * @brief Phong light model program
     *
     */
    class PhongProgram : public Program, public ProgramMatrix, public ProgramMaterial, public ProgramLight
    {
    public:
        virtual ~PhongProgram() = default;
        //
        //  Create Phong Program shared ptr
        //
        static std::shared_ptr<PhongProgram>
        create(std::string_view vs_file_name, std::string_view fs_file_name);
    };

    using phong_program_ptr = std::shared_ptr<PhongProgram>;

    class EnvironmentMapProgram : public Program, public ProgramMaterial, public ProgramMatrix, public ProgramLight
    {
        //
        //  Create Environment Map Program shared ptr
        //
        static std::shared_ptr<EnvironmentMapProgram>
        create(std::string_view vs_file_name, std::string_view fs_file_name);
    };

    using env_map_program_ptr = std::shared_ptr<EnvironmentMapProgram>;

    class SkinnedProgram : public Program,
                           public ProgramMaterial,
                           public ProgramMatrix,
                           public ProgramLight,
                           public ProgramSkinned
    {
    public:
        virtual ~SkinnedProgram() = default;
        //
        //  Create skinned mesh progrom shared pointer
        //
        static std::shared_ptr<SkinnedProgram>
        create(std::string_view vs_file_name, std::string_view fs_file_name);
    };

    using skinned_program_ptr = std::shared_ptr<SkinnedProgram>;

    /**
     * @brief Skybox Program
     *
     */
    struct SkyboxProgram : public Program,
                           public ProgramMatrix
    {
        virtual ~SkyboxProgram() = default;

        static std::shared_ptr<SkyboxProgram>
        create(std::string_view vs_file_name, std::string_view fs_file_name);
    };

    using skybox_program_ptr = std::shared_ptr<SkyboxProgram>;

    //
    // Program Pool
    //
    struct ProgramPool
    {
        virtual ~ProgramPool() = default;

        virtual std::vector<std::string_view> list() = 0;

        virtual program_ptr get_program(std::string_view program_name) = 0;
    };

    using program_factory_ptr = std::shared_ptr<ProgramPool>;
}