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
    
        InputConnector connector;
        connector.index = i;
        connector.relative_pos = ImVec2(10.0f + (i * 40.0f), -4.0f);
        m_InputConnectors.push_back(connector);    
    }
}
