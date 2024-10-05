#ifndef NODE_EDIRTOR_H
#define NODE_EDITOR_H
#pragma once


#include <glad/glad.h>
#include <glfw/glfw3.h>
#define GLFW_MOD_CTRL_SHIFT 0x0003
#define GLFW_MOD_SHIFT_ALT 0x0005
#define GLFW_MOD_CTRL_ALT 0x0006



#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>



#include "ui_utils.h"
#include "ImGuiNode.h"
#include "NodeParam.h"
#include "NodeManager.h"
#include "Application.h"

#include "Event.h"
#include "EventManager.h"
#include "EventDispatcher.h"


template<typename T>
void node_menu_item(NodeEditor::NodeManager &manager, const char* label) {
    if (ImGui::MenuItem(label, NULL, false, true)) {
      manager.AddNode(std::make_shared<T>(label));
    }
};
#endif