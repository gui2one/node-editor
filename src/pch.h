#ifndef NODE_EDITOR_PCH_H
#define NODE_EDITOR_PCH_H
#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <filesystem>
#include <typeinfo>
#include <chrono>
#include <assert.h>
#include <random>

// include windows before GLFW to avoid compilation warnings
#if defined(_WIN32) 
#define NOMINMAX
#include <windows.h>
#endif

#define GLFW_INCLUDE_NONE 
#include <glfw/glfw3.h>
#include <glad/glad.h>
#define GLFW_MOD_CTRL_SHIFT 0x0003
#define GLFW_MOD_SHIFT_ALT 0x0005
#define GLFW_MOD_CTRL_ALT 0x0006




#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h> /* for std::string in ImGui::InputText() */

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "yaml_convert.h"


#endif