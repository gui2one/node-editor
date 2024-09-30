#include "ImGuiNode.h"

ImGuiNode::ImGuiNode(const char * _title) : title(_title), position(500, 500), size(100, 30)
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

    if(GetInput(0) != nullptr) {
        ImVec2 nodePos = ImVec2(position.x + offset.x + size.x/2.0f, position.y + offset.y);
        ImVec2 inputPos = ImVec2(GetInput(0)->position.x + offset.x + GetInput(0)->size.x/2.0f, GetInput(0)->position.y + offset.y + GetInput(0)->size.y);
        float y_sep = inputPos.y - nodePos.y;
        ImVec2 ctrl1 = ImVec2(nodePos.x, nodePos.y + y_sep);
        ImVec2 ctrl2 = ImVec2(inputPos.x, inputPos.y - y_sep);

        draw_list->AddBezierCubic(nodePos, ctrl1, ctrl2, inputPos, IM_COL32(0, 255, 255, 255), 2.0f); // ImDrawList API uses screen coordinates()
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
