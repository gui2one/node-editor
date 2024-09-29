#include "NodeManager.h"

NodeManager::NodeManager()
{
    
}


NodeManager::~NodeManager()
{

}

void NodeManager::MouseEvents()
{
    ImGuiNode* hovered = nullptr;
    ImGuiNode* cur_node = nullptr;    
    for(auto node : nodes) {
        node->highlighted = false;
        // node->selected = false;
        if(node->IsHovered()) {
            if(ImGui::IsMouseClicked(0)) {
                if(node->selected) {
                    node->selected = false;
                }else{

                    cur_node = node.get();
                }
            }

            hovered = node.get();
        }
    }
    
    if( cur_node){

        cur_node->selected = true;
    }
    if(hovered) {
        hovered->highlighted = true;
    }
}
