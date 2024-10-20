#include "yaml_serialize.h"

YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::ImGuiNode>& node) {
  out << node->YAMLSerialize();
  return out;
}
namespace NodeEditor {

YAML::Node serialize_network(NodeNetwork &network)
{
  YAML::Node output;
  if(network.outuput_node != nullptr){

    output["output_node"] = network.outuput_node->uuid;
  }else{
    output["output_node"] = "null";

  }
  output["nodes"] = serialize_nodes(network.nodes);

  return output;
}

YAML::Node serialize_nodes(std::vector<std::shared_ptr<ImGuiNode>> nodes) {

  YAML::Node output;
  for(auto node : nodes) {
    output.push_back(node->YAMLSerialize());
  }


  return output;
}

void save_all(std::filesystem::path path, NodeNetwork &network)
{
  std::fstream saved_file(path.string(), std::ios::out);

  saved_file << YAML::Dump(serialize_network(network));
  // saved_file << serialize_nodes(network.nodes);
  saved_file.close();
  
}

std::shared_ptr<ImGuiNode> deserialize_node(YAML::Node yaml_node) {

    std::string type_name = yaml_node["type"].as<std::string>();

    auto factory_node = NodeFactoryRegistry::instance().create(type_name);
    if(factory_node == nullptr) {
      std::cout << "Unabled to create type: " << type_name << "" << std::endl;
      
      return nullptr;
    }
    factory_node->position = yaml_node["position"].as<ImVec2>();
    factory_node->title = yaml_node["title"].as<std::string>();
    factory_node->uuid = yaml_node["uuid"].as<std::string>();

    for(size_t i= 0; i<yaml_node["params"].size(); i++) {
      auto p_node = yaml_node["params"][i];
      std::string p_type_str = p_node["type"].as<std::string>();
      auto param = factory_node->m_ParamLayout.items[i].param;
      
      if(p_type_str == "std::string" || p_type_str.find("std::basic_string") != std::string::npos) {
        set_param_value<std::string>(param, p_node["value"].as<std::string>());
      }else if(p_type_str == "unsigned int") {
        set_param_value<unsigned int>(param, p_node["value"].as<unsigned int>());
      }else if(p_type_str == "float") {
        set_param_value<float>(param, p_node["value"].as<float>());
      }else if(p_type_str == "bool") {
        set_param_value<bool>(param, p_node["value"].as<bool>()); 
      }else if(p_type_str.find("struct glm::vec<2,float,0>") != std::string::npos) {
        set_param_value<glm::vec2>(param, p_node["value"].as<glm::vec2>()); 
      }else if(p_type_str.find("struct glm::vec<3,float,0>") != std::string::npos) {
        set_param_value<glm::vec3>(param, p_node["value"].as<glm::vec3>()); 
      }else if(p_type_str.find("ParamComboBox") != std::string::npos) {
        auto combo_p = std::dynamic_pointer_cast<ParamComboBox>(param);
        combo_p->SetChoice(p_node["value"].as<int>());
      }  
    }

    auto subnet_ptr = std::dynamic_pointer_cast<SubnetNode>(factory_node);
    if( subnet_ptr != nullptr){
      std::cout << "TRYING TO deserialize a subnet OPERATOR" << std::endl;
      
      
      NODE_COLLECTION children;
      
      subnet_ptr->position = yaml_node["position"].as<ImVec2>();
      subnet_ptr->title = yaml_node["title"].as<std::string>();
      subnet_ptr->uuid = yaml_node["uuid"].as<std::string>();
      
      for(auto yaml_subnet_node : yaml_node["children"]) {
        auto nd = deserialize_node(yaml_subnet_node);
        if(nd != nullptr){
          children.push_back(nd);
        }
      }

      subnet_ptr->node_network.nodes = children;

      return subnet_ptr;
    }
    return factory_node;
}

std::vector<std::shared_ptr<ImGuiNode>> deserialize_nodes(YAML::Node yaml) {

  // YAML::Node output = YAML::Load(yaml);
  std::vector<std::shared_ptr<ImGuiNode>> nodes;
  for(auto node : yaml) {

    auto factory_node = deserialize_node(node);
    if(factory_node != nullptr){
      nodes.push_back(factory_node);
    }
  }

  // second pass to make connections
  for(auto node : yaml) {
    auto my_uuid = node["uuid"].as<std::string>();
    auto my_self = Utils::FindNodeByUUID(my_uuid, nodes);
    
    if(my_self == nullptr) continue;
    
    for(size_t i=0; i< MAX_N_INPUTS; i++) {
      auto input_uuid = node["inputs"][i].as<std::string>();
      if(input_uuid == "null") continue;
      auto input_node = Utils::FindNodeByUUID(input_uuid, nodes);

      if(input_node == nullptr) continue;

      my_self->SetInput((uint32_t)i, input_node);

    }
    for(size_t i=0; i< node["multi_input"].size(); i++) {
      auto input_uuid = node["multi_input"][i].as<std::string>();
      auto input_node = Utils::FindNodeByUUID(input_uuid, nodes);

      if(input_node == nullptr) continue;

      my_self->AppendInput(input_node);

    }
  }
  return nodes;
}

std::vector<std::shared_ptr<ImGuiNode>> load_yaml_file(std::filesystem::path path)
{
  std::ifstream saved_file(path.string());
  std::string content((std::istreambuf_iterator<char>(saved_file)), std::istreambuf_iterator<char>());  
  std::vector<std::shared_ptr<ImGuiNode>> nodes;
  YAML::Node output = YAML::Load(content);

  nodes = deserialize_nodes(output);
  return nodes;
}
};

