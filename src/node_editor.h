#ifndef NODE_EDITOR_H
#define NODE_EDITOR_H
#pragma once

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