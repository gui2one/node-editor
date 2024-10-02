#include "ImGuiNode.h"

ImGuiNode::ImGuiNode(const char * _title) : title(_title), position(500, 500), size(100, 30), color(NODE_COLOR::DARK_GREY)
{

}

ImGuiNode::~ImGuiNode()
{

}
