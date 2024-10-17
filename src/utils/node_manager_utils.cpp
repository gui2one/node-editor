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

    ImVec2 get_nodes_center(std::vector<std::shared_ptr<ImGuiNode>>& nodes) {
        if (nodes.size() == 0)
            return ImVec2(0, 0);
        float minx = 999999999.f, miny = 999999999.f, maxx = -999999999.f,
                maxy = -999999999.f;

        for (auto node : nodes) {
            if (node->position.x < minx)
            minx = node->position.x;
            if (node->position.y < miny)
            miny = node->position.y;
            if (node->position.x > maxx)
            maxx = node->position.x;
            if (node->position.y > maxy)
            maxy = node->position.y;
        }
        float centerx, centery;
        centerx = (minx + maxx) / 2.0f + nodes[0]->size.x / 2.0f;
        centery = (miny + maxy) / 2.0f + nodes[0]->size.y / 2.0f;
        return ImVec2(centerx, centery);
    }
    void deselect_all(std::vector<std::shared_ptr<ImGuiNode>> &nodes)
    {
        for(auto node : nodes){
            node->selected = false;
        }
        // return ImVec2();
    }
};