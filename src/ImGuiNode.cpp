#include "ImGuiNode.h"

ImGuiNode::ImGuiNode() : title("default"), position(500, 500), size(100, 30)
{

}

ImGuiNode::~ImGuiNode()
{

}

void ImGuiNode::render(ImDrawList *draw_list, ImVec2 offset)
{
    ImVec2 min = ImVec2(position.x + offset.x, position.y + offset.y);
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);
    draw_list->AddRectFilled(min, max, IM_COL32(30, 30, 30, 255), 3.0f);

    bool hovered = ImGui::IsMouseHoveringRect(min, max);
    if(hovered){
        draw_list->AddRect(min, max, IM_COL32(50, 50, 50, 255), 3.0f);
    } else {
        draw_list->AddRect(min, max, IM_COL32(100, 100, 100, 255), 3.0f);
    }
    
}
