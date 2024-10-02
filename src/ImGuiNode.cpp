#include "ImGuiNode.h"

ImGuiNode::ImGuiNode(const char * _title) : title(_title), position(500, 500), size(100, 30), color(NODE_COLOR::DARK_GREY)
{

}

ImGuiNode::~ImGuiNode()
{

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
