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
    
    /**
     * @brief Phong light model program
     *
     */
    class PhongProgram : public Program, public ProgramMatrix, public ProgramMaterial, public ProgramLight
    {
    public:
        virtual ~PhongProgram() = default;

        // //
        // //  Program methods
        // //
        // virtual void apply() override;
        // //
        // //  Camera settings
        // //
        // virtual void set_world_matrix(const glm::mat4 &world) override;

        // virtual void set_view_matrix(const glm::mat4 &view) override;

        // virtual void set_project_matrix(const glm::mat4 &project) override;

        // virtual void set_mvp_matrices(glm::mat4 m, glm::mat4 v, glm::mat4 p) override;
        // //
        // //  Material settings
        // //
        // virtual void set_material(material_ptr materail) override;
        // //
        // //  Light settings
        // //
        // virtual void set_direction_light(const DirectionLight &dir_light) override;

        // virtual void set_point_light(size_t index, const PointLight &point_light) override;

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
}