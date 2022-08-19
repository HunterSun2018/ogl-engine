#pragma once
#include "pch.hpp"

namespace ogle
{
    void GLAPIENTRY check_error(GLenum error);
    void GLAPIENTRY gl_debug_output(GLenum src, GLenum type, GLuint id, GLenum severity,
                                    GLsizei len, const char *msg, const void *cls);
}
