#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once
#include "ImGuiNode.h"
namespace NodeEditor::Utils{

    struct FileFilterItem{
        std::string description;
        std::string extension;
    };
    
    
    std::shared_ptr<NodeEditor::AbstractNode> 
    FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<NodeEditor::AbstractNode>> nodes);
    
    ImVec2 get_nodes_center(std::vector<std::shared_ptr<AbstractNode>>& nodes);
    void deselect_all(std::vector<std::shared_ptr<AbstractNode>>& nodes);

    std::filesystem::path open_file_explorer(std::vector<FileFilterItem> filters = {{ "All Files", "*.*" }});
    const char *create_windows_file_explorer_Filter(std::vector<FileFilterItem> filters);
};
#endif // NODE_EDITOR_NODE_MANAGER_UTILS_H