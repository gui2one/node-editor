#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once
#include "ImGuiNode.h"
namespace NodeEditor::Utils{
    
    std::shared_ptr<NodeEditor::ImGuiNode> 
    FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes);
    
    ImVec2 get_nodes_center(std::vector<std::shared_ptr<ImGuiNode>>& nodes);
    void deselect_all(std::vector<std::shared_ptr<ImGuiNode>>& nodes);

    std::filesystem::path open_file_explorer();
};
#endif // NODE_EDITOR_NODE_MANAGER_UTILS_H