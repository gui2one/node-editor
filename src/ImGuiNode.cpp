#include "ImGuiNode.h"

ImGuiNode::ImGuiNode() : title("default"), position(500, 500), size(100, 30)
{

}

ImGuiNode::~ImGuiNode()
{

}

void ImGuiNode::Render(ImDrawList *draw_list, ImVec2 _offset)
{
    offset = _offset;
    ImVec2 min = ImVec2(position.x + offset.x, position.y + offset.y);
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);
    draw_list->AddRectFilled(min, max, IM_COL32(30, 30, 30, 255), 3.0f);

    draw_list->AddText(ImVec2(min.x + 10, min.y + 10), IM_COL32(255, 255, 255, 255), title);   
    if(!highlighted){
        draw_list->AddRect(min, max, IM_COL32(50, 50, 50, 255), 3.0f);
    } else {
        draw_list->AddRect(min, max, IM_COL32(100, 100, 100, 255), 3.0f);
    }

    if(selected){
        draw_list->AddRect(min, max, IM_COL32(200, 200, 60, 100), 3.0f, 0, 3.0f);
    }

}

bool ImGuiNode::IsHovered()
{
    ImVec2 min = ImVec2(position.x + offset.x, position.y + offset.y);
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);    
    bool hovered = ImGui::IsMouseHoveringRect(min, max);
    return hovered;
}

bool ImGuiNode::IsNodeDragged()
{
    return false;
}
