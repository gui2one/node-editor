#ifndef NODE_EDIRTOR_H
#define NODE_EDITOR_H
#pragma once


#include <glad/glad.h>
#include <glfw/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ImGuiNode.h"
#include "NodeManager.h"
#include "Application.h"

#endif