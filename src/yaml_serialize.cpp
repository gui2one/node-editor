#include "yaml_serialize.h"

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

    std::string type_name = node["type"].as<std::string>();

    auto factory_node = NodeEditor::NodeFactoryRegistry::instance().create(type_name);
    if(factory_node == nullptr) {
      std::cout << "Unabled to create type: " << type_name << "" << std::endl;
      
      continue;
    }
    factory_node->position = node["position"].as<ImVec2>();
    factory_node->title = node["title"].as<std::string>();
    factory_node->uuid = node["uuid"].as<std::string>();

    for(size_t i= 0; i<node["params"].size(); i++) {
      auto p_node = node["params"][i];
      std::string p_type_str = p_node["type"].as<std::string>();
      auto param = factory_node->m_ParamLayout.items[i].param;
      
      if(p_type_str == "std::string" || p_type_str.find("std::basic_string") != std::string::npos) {
        NodeEditor::set_param_value<std::string>(param, p_node["value"].as<std::string>());
      }else if(p_type_str == "unsigned int") {
        NodeEditor::set_param_value<unsigned int>(param, p_node["value"].as<unsigned int>());
      }else if(p_type_str == "float") {
        NodeEditor::set_param_value<float>(param, p_node["value"].as<float>());
      }else if(p_type_str == "bool") {
        NodeEditor::set_param_value<bool>(param, p_node["value"].as<bool>()); 
      }else if(p_type_str.find("struct glm::vec<2,float,0>") != std::string::npos) {
        NodeEditor::set_param_value<glm::vec2>(param, p_node["value"].as<glm::vec2>()); 
      }else if(p_type_str.find("struct glm::vec<3,float,0>") != std::string::npos) {
        NodeEditor::set_param_value<glm::vec3>(param, p_node["value"].as<glm::vec3>()); 
      }else if(p_type_str.find("ParamComboBox") != std::string::npos) {
        auto combo_p = std::dynamic_pointer_cast<NodeEditor::ParamComboBox>(param);
        combo_p->SetChoice(p_node["value"].as<int>());
      }  
    }
    
    nodes.push_back(factory_node);
  }

  // second pass to make connections
  for(auto node : output) {
    auto my_uuid = node["uuid"].as<std::string>();
    auto my_self = NodeEditor::Utils::FindNodeByUUID(my_uuid, nodes);
    
    if(my_self == nullptr) continue;
    
    for(size_t i=0; i< MAX_N_INPUTS; i++) {
      auto input_uuid = node["inputs"][i].as<std::string>();
      if(input_uuid == "null") continue;
      auto input_node = NodeEditor::Utils::FindNodeByUUID(input_uuid, nodes);

      if(input_node == nullptr) continue;

      my_self->SetInput((uint32_t)i, input_node);

    }
    for(size_t i=0; i< node["multi_input"].size(); i++) {
      auto input_uuid = node["multi_input"][i].as<std::string>();
      auto input_node = NodeEditor::Utils::FindNodeByUUID(input_uuid, nodes);

      if(input_node == nullptr) continue;

      my_self->AppendInput(input_node);

    }
  }
  return nodes;
}


