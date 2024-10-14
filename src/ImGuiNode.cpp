#include "ImGuiNode.h"
#include "yaml_serialize.h"
namespace NodeEditor {


ImGuiNode::ImGuiNode(std::string _title)
    : title(_title), position(500, 500), size(100, 30),
      color(NODE_COLOR::DARK_GREY) {


    uuid = generate_uuid();        
}

ImGuiNode::~ImGuiNode() {}

void ImGuiNode::InitInputConnectors() {
  m_InputConnectors.clear();
  uint32_t num_spots = GetNumAvailableInputs();

  if (num_spots == 0)
    return;

  float spot_width = size.x / num_spots;
  for (uint32_t i = 0; i < num_spots; i++) {
    float x = spot_width * i + spot_width / 2.0f;
    float y = -4.0f;
    InputConnector connector;
    connector.index = i;
    connector.relative_pos = ImVec2(x, y);
    m_InputConnectors.push_back(connector);
  }
}

YAML::Node ImGuiNode::YAMLSerialize() { 
  YAML::Node yaml_node;
  yaml_node["title"] = title;
  yaml_node["position"] = position;
  yaml_node["size"] = size;
  return yaml_node; 
  
}

void ImGuiNode::SetInput(uint32_t index, std::shared_ptr<ImGuiNode> node) {
  if (index < 0 || index > 3)
    return;
  inputs[index] = node;
}

void ImGuiNode::ResetInput(uint32_t index){
    if (index < 0 || index > 3)
        return;
    inputs[index] = nullptr;
}
std::shared_ptr<ImGuiNode> ImGuiNode::GetInput(uint32_t index)
{
    if (index < 0 || index > 3)
        return nullptr;
    return inputs[index];
}


InputConnector* ImGuiNode::GetInputConnector(uint32_t index)
{
    if (index < 0 || index >= GetNumAvailableInputs())
    {
        std::cout << "Problem with GetInputConnector" << std::endl;

        return nullptr;
    }
    return &m_InputConnectors[index];
}

void OUTPUT_NODE::Update() {
  if (GetInput(0) != nullptr) {
    GetInput(0)->Update();
  }
}

ParamLayoutItem& ParamLayout::Get(size_t idx) {

  return items[idx];
}

}; // namespace NodeEditor