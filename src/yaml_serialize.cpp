#include "yaml_serialize.h"

YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::ImGuiNode>& node) {

  out << node->YAMLSerialize();
  return out;
}


std::string serialize_nodes(std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes) {

  YAML::Node output;
  for(auto node : nodes) {
    output.push_back(node->YAMLSerialize());
  }
  return YAML::Dump(output);
}

std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> deserialize_nodes(std::string yaml) {

  std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes;
  YAML::Node output = YAML::Load(yaml);
  for(auto node : output) {
    // std::cout << YAML::Dump(node) << std::endl;
    std::string type_name = node["type"].as<std::string>();
    std::cout << "Deserializing type : " << type_name << std::endl;
    auto factory_node = NodeEditor::NodeFactoryRegistry::instance().create(type_name);
    factory_node->position = node["position"].as<ImVec2>();
    nodes.push_back(factory_node);
  }
  return nodes;
}


