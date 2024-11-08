#ifndef NODE_EDITOR_PCH_H
#define NODE_EDITOR_PCH_H
#pragma once

#include <assert.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <typeinfo>
#include <vector>


// include windows before GLFW to avoid compilation warnings
#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <glfw/glfw3.h>

#define GLFW_MOD_CTRL_SHIFT 0x0003
#define GLFW_MOD_SHIFT_ALT 0x0005
#define GLFW_MOD_CTRL_ALT 0x0006

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_DEFINE_MATH_OPERATORS
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h> /* for std::string in ImGui::InputText() */


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "constants.h"
#include "yaml_convert.h"


#endif