/**
 * @file pch.hpp
 * @author Hunter (HunterSun2018@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
//
//  Header files for std
//
#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <list>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <vector>

// https://github.com/fmtlib/fmt
#include <fmt/core.h>

//
//  Header files for openGL
//
#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>