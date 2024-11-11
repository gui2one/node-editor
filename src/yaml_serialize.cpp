#include "yaml_serialize.h"

YAML::Emitter& operator<<(YAML::Emitter& out, const std::shared_ptr<NodeEditor::AbstractNode>& node) {
  out << node->YAMLSerialize();
  return out;
}
namespace NodeEditor {

YAML::Node serialize_network(NodeNetwork network) {
  YAML::Node output;
  if (network.outuput_node != nullptr) {
    output["output_node"] = network.outuput_node->uuid;
  } else {
    output["output_node"] = "null";
  }
  output["nodes"] = serialize_nodes(network.nodes);

  return output;
}

YAML::Node serialize_nodes(std::vector<std::shared_ptr<AbstractNode>> nodes) {
  YAML::Node output;
  for (auto node : nodes) {
    if (node->m_Serializable) {
      output.push_back(node->YAMLSerialize());
    }
  }
  return output;
}

void save_all(std::filesystem::path path, NodeNetwork& network) {
  std::fstream saved_file(path.string(), std::ios::out);

  saved_file << YAML::Dump(serialize_network(network));
  // saved_file << serialize_nodes(network.nodes);
  saved_file.close();
}

std::shared_ptr<AbstractNode> deserialize_node(YAML::Node yaml_node) {
  std::string type_name = yaml_node["type"].as<std::string>();

  auto factory_node = NodeFactoryRegistry::GetInstance().Create(type_name);
  if (factory_node == nullptr) {
    std::cout << "Unabled to create type: " << type_name << "" << std::endl;
    return nullptr;
  }

  factory_node->position = yaml_node["position"].as<ImVec2>();
  factory_node->title = yaml_node["title"].as<std::string>();
  factory_node->uuid = yaml_node["uuid"].as<std::string>();

  bool is_subnet = yaml_node["is_subnet"].as<bool>();

  if (is_subnet) {
    factory_node->ActivateSubnet();
  }

  for (size_t i = 0; i < yaml_node["params"].size(); i++) {
    auto p_node = yaml_node["params"][i];

    if (p_node["type"].as<std::string>() == "ParamGroup") {
      for (size_t j = 0; j < p_node["params"].size(); j++) {
        deserialize_param(p_node["params"][j], factory_node);
      }
    } else {
      deserialize_param(p_node, factory_node);
    }
  }

  if (is_subnet) {
    NodeNetwork net;
    net = deserialize_network(yaml_node["node_network"]);
    // add the nodes rather than set nodes to std::vector<nodes> because this overwrites subnetinputnodes created
    // inside node Constructor
    for (auto node : net.nodes) {
      node->parent_node = factory_node.get();
      factory_node->node_network.AddNode(node);
      factory_node->node_network.outuput_node = net.outuput_node;
    }
  }
  return factory_node;
}

std::shared_ptr<NodeParam> find_param_by_name(std::shared_ptr<AbstractNode> factory_node, std::string param_name) {
  for (auto& param_item : factory_node->m_ParamLayout.params) {
    auto p_group = std::dynamic_pointer_cast<ParamGroup>(param_item);
    if (p_group != nullptr) {
      if(p_group->name == param_name) {
        return p_group;
      }
      for (auto group_item : p_group->items) {
        if (group_item->name == param_name) {
          return group_item;
        }
      }

      
    } else {
      if (param_item->name == param_name) {
        return param_item;
      }
    }
  }

  return nullptr;
}

void deserialize_param(YAML::Node yaml, std::shared_ptr<AbstractNode> factory_node) {
  std::string p_type_str = yaml["type"].as<std::string>();
  std::string p_name = yaml["name"].as<std::string>();

  str_replace_all(p_type_str, "NodeEditor::Param<", "");
  str_remove_last(p_type_str, ">");
  str_replace_all(p_type_str, "NodeEditor::", "");

  std::shared_ptr<NodeParam> param = nullptr;

  param = find_param_by_name(factory_node, p_name);
  if (param == nullptr) {
    std::cout << "Could not find param: " << p_name << "" << std::endl;
    
    return;
  }
  if (p_type_str == "std::string") {
    set_param_value<std::string>(param, yaml["value"].as<std::string>());

  } else if (p_type_str == "int") {
    set_param_value<int>(param, yaml["value"].as<int>());

  } else if (p_type_str == "float") {
    set_param_value<float>(param, yaml["value"].as<float>());

  } else if (p_type_str == "bool") {
    set_param_value<bool>(param, yaml["value"].as<bool>());

  } else if (p_type_str == "glm::vec<2,float,0>") {
    set_param_value<glm::vec2>(param, yaml["value"].as<glm::vec2>());

  } else if (p_type_str == "glm::vec<3,float,0>") {
    set_param_value<glm::vec3>(param, yaml["value"].as<glm::vec3>());

  } else if (p_type_str == "ParamComboBox") {
    auto combo_p = std::dynamic_pointer_cast<ParamComboBox>(param);
    combo_p->SetChoice(yaml["value"].as<int>());

  } else if (p_type_str == "ParamGroup") {
    auto group_p = std::dynamic_pointer_cast<ParamGroup>(param);
    for (size_t j = 0; j < yaml["params"].size(); j++) {
      deserialize_param(yaml["params"][j], factory_node);
    }

  } else if (p_type_str == "ParamFile") {
    auto file_p = std::dynamic_pointer_cast<ParamFile>(param);
    file_p->value = utf8_to_wide(yaml["value"].as<std::string>());
  }
}

NodeNetwork deserialize_network(YAML::Node yaml) {
  NodeNetwork network;

  network.nodes = deserialize_nodes(yaml["nodes"]);
  std::string output_node_uuid = yaml["output_node"].as<std::string>();
  if (output_node_uuid != "null") {
    auto output_node = Utils::FindNodeByUUID(output_node_uuid, network.nodes);
    if (output_node != nullptr) {
      network.outuput_node = output_node;
    } else {
      std::cout << "NO OUTPUT NODE FOUND !!!!!!!!!!" << std::endl;
    }
  }
  return network;
}

std::vector<std::shared_ptr<AbstractNode>> deserialize_nodes(YAML::Node yaml) {
  std::vector<std::shared_ptr<AbstractNode>> nodes;

  for (auto node : yaml) {
    auto factory_node = deserialize_node(node);
    if (factory_node != nullptr) {
      nodes.push_back(factory_node);
    }
  }

  // second pass to make connections
  for (auto node : yaml) {
    auto my_uuid = node["uuid"].as<std::string>();
    auto my_self = Utils::FindNodeByUUID(my_uuid, nodes);

    if (my_self == nullptr) continue;

    for (size_t i = 0; i < MAX_N_INPUTS; i++) {
      auto input_uuid = node["inputs"][i].as<std::string>();

      if (input_uuid == "null") continue;
      auto input_node = Utils::FindNodeByUUID(input_uuid, nodes);

      if (input_node == nullptr) {
        continue;
      }

      my_self->SetInput((uint32_t)i, input_node);
    }
    for (size_t i = 0; i < node["multi_input"].size(); i++) {
      auto input_uuid = node["multi_input"][i].as<std::string>();
      auto input_node = Utils::FindNodeByUUID(input_uuid, nodes);

      if (input_node == nullptr) continue;

      my_self->AppendInput(input_node);
    }
  }
  return nodes;
}

struct post_load_connection {
  std::string uuid;
  int input_index;
  int subnet_input_node_index;
};

std::vector<post_load_connection> collect_subnet_connections(YAML::Node yaml) {
  std::vector<post_load_connection> connections;
  for (auto node : yaml["nodes"]) {
    if (node["is_subnet"].as<bool>()) {
      auto _connections = collect_subnet_connections(node["node_network"]);
      connections.insert(connections.end(), _connections.begin(), _connections.end());
      for (auto subnet_node : node["node_network"]["nodes"]) {
        if (subnet_node["inputs"][0].as<std::string>() == "opinput_0") {
          connections.push_back({subnet_node["uuid"].as<std::string>(), 0, 0});
        }
      }
    }
  }
  return connections;
}

void apply_subnet_connections(NodeNetwork& network, std::vector<post_load_connection> connections) {
  for (auto node : network.nodes) {
    if (node->IsSubnet()) {
      apply_subnet_connections(node->node_network, connections);
      for (auto subnet_node : node->node_network.nodes) {
        for (auto conn : connections) {
          if (conn.uuid == subnet_node->uuid) {
            subnet_node->SetInput((uint32_t)conn.input_index,
                                  Utils::FindNodeByUUID("opinput_0", node->node_network.nodes));
          }
        }
      }
    }
  }
}

NodeNetwork load_yaml_file(std::filesystem::path path) {
  std::ifstream saved_file(path.string());
  std::string content((std::istreambuf_iterator<char>(saved_file)), std::istreambuf_iterator<char>());
  std::vector<std::shared_ptr<AbstractNode>> nodes;
  YAML::Node output = YAML::Load(content);

  auto network = deserialize_network(output);
  auto connections = collect_subnet_connections(output);
  apply_subnet_connections(network, connections);

  return network;
}
};  // namespace NodeEditor
