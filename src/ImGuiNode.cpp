#include "ImGuiNode.h"

ImGuiNode::ImGuiNode(const char * _title) : title(_title), position(500, 500), size(100, 30), color(NODE_COLOR::DARK_GREY)
{

}

ImGuiNode::~ImGuiNode(){

}

void ImGuiNode::InitInputConnectors()
{
    m_InputConnectors.clear();
    uint32_t num_spots = GetNumAvailableInputs();
    
    if (num_spots == 0)  return; 

    float spot_width = size.x / num_spots;
    for(uint32_t i = 0; i < num_spots; i++) {
        float x = spot_width * i + spot_width / 2.0f;
        float y = -4.0f;
        InputConnector connector;
        connector.index = i;
        connector.relative_pos = ImVec2(x, y);
        m_InputConnectors.push_back(connector);    
    }
}
