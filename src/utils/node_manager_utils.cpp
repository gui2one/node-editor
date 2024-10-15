#include "node_manager_utils.h"

namespace NodeEditor::Utils {
    std::shared_ptr<NodeEditor::ImGuiNode> FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes)
    {
        for(auto node : nodes){
            if(node->uuid == uuid){
                return node;
            }
        }
        return nullptr;
    }
}