#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once
#include "ImGuiNode.h"
namespace NodeEditor::Utils{
    std::shared_ptr<NodeEditor::ImGuiNode> FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes);
};
#endif // NODE_EDITOR_NODE_MANAGER_UTILS_H