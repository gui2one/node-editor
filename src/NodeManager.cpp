#include "NodeManager.h"

NodeManager::NodeManager()
{
    
}


NodeManager::~NodeManager()
{

}


void ungrab_all(std::vector<std::shared_ptr<ImGuiNode>>& nodes) {
    for(auto node : nodes) {
        node->grabbed = false;
    }
}

void NodeManager::MouseEvents()
{

    static int inc = 0;
    ImGuiNode* hovered = nullptr;
    ImGuiNode* cur_node = nullptr;    

    for(auto node : nodes) {
        node->highlighted = false;
        // node->selected = false;
        if(ImGui::IsMouseClicked(0) && node->IsHovered()) {
            
            if(node->selected) {
                node->selected = false;
            }else{
                cur_node = node.get();
            }
        }
        if(ImGui::IsMouseDragging(ImGuiMouseButton_Left, 1.0f)) {
            if(node->IsHovered()) {
                if( node->grabbed == false) {
                    
                    node->grabbed = true;    
                }
                hovered = node.get();
            }  
        }else{
            node->grabbed = false;
        }




        
    }
    
    if( cur_node){

        cur_node->selected = true;      
    }
    if(hovered) {
        hovered->highlighted = true;
    }

    for(auto node : nodes) {
        if(node->grabbed) {
            node->position.x += ImGui::GetIO().MouseDelta.x;
            node->position.y += ImGui::GetIO().MouseDelta.y;
        }
    }
}

