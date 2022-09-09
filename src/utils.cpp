#include "pch.hpp"

using namespace std;

#define ENUM2STR(x) \
    case (x):       \
        return (#x)

namespace ogle
{
    void GLAPIENTRY gl_debug_output(GLenum src, GLenum type, GLuint id, GLenum severity,
                                    GLsizei len, const char *msg, const void *cls)
    {
        //if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        //    return;
        if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
            return;

        auto enum_to_str = [](GLenum type) -> string
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
                // Severity
                ENUM2STR(GL_DEBUG_SEVERITY_HIGH);
                ENUM2STR(GL_DEBUG_SEVERITY_MEDIUM);
                ENUM2STR(GL_DEBUG_SEVERITY_LOW);
                ENUM2STR(GL_DEBUG_SEVERITY_NOTIFICATION);
            default:
                return "unkonw";
            }
        };

        cerr << fmt::format("[GLDEBUG] {} {} {}: {}", enum_to_str(src), enum_to_str(type), enum_to_str(severity), msg) << endl;       
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
}