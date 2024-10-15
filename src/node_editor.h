#ifndef NODE_EDITOR_H
#define NODE_EDITOR_H

#include "yaml_serialize.h"
#include "ImGuiNode.h"
#include "NodeParam.h"
#include "NodeManager.h"
#include "NodeFactory.h"
#include "Application.h"

#include "Event.h"
#include "EventManager.h"
#include "EventDispatcher.h"


template<typename T>
void node_menu_item(NodeEditor::NodeManager &manager, const char* label) {
    if (ImGui::MenuItem(label, NULL, false, true)) {
      auto node = std::make_shared<T>(label);
      double x,y;
      glfwGetCursorPos(manager.GetGLFWWindow(), &x, &y);

      node->position = ImVec2((float)x, (float)y) - manager.m_ViewProps.scrolling - manager.m_ViewProps.canvasPos;
      manager.AddNode(node);
    }
};
#endif