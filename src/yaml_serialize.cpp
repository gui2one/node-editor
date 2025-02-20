#include "yaml_serialize.h"

#include "utils/node_manager_utils.h"

namespace NED {

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
  factory_node->color = (NODE_COLOR)yaml_node["color"].as<ImU32>();

  bool is_subnet = yaml_node["is_subnet"].as<bool>();

  if (is_subnet) {
    factory_node->ActivateSubnet();
  }

  for (size_t i = 0; i < yaml_node["params"].size(); i++) {
    auto p_node = yaml_node["params"][i];

    if (p_node["type"].as<std::string>() == "NED::ParamGroup") {
      for (size_t j = 0; j < p_node["params"].size(); j++) {
        // std::cout << "Deserializing group param: " << p_node["params"][j]["label"].as<std::string>() << std::endl;
        auto param = Utils::FindParamByName(factory_node.get(), p_node["params"][j]["label"].as<std::string>());
        param->YAMLDeserialize(p_node["params"][j]);
      }
    } else {
      auto param = Utils::FindParamByName(factory_node.get(), p_node["label"].as<std::string>());
      param->YAMLDeserialize(p_node);
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
    }
    factory_node->node_network.outuput_node = net.outuput_node;
  }
  return factory_node;
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
      auto input_uuid = node["inputs"][i]["node"].as<std::string>();

      if (input_uuid == "null") continue;
      auto input_node = Utils::FindNodeByUUID(input_uuid, nodes);

      if (input_node == nullptr) {
        continue;
      }

      my_self->SetInput((uint32_t)i, input_node.get());
    }
    for (size_t i = 0; i < node["multi_input"].size(); i++) {
      auto input_uuid = node["multi_input"][i]["node"].as<std::string>();
      auto input_node = Utils::FindNodeByUUID(input_uuid, nodes);

      if (input_node == nullptr) continue;

      my_self->AppendInput(input_node.get());
    }
  }
  return nodes;
}

struct post_load_connection {
  std::string uuid;
  int input_index;
  int subnet_input_node_index;
};

NodeNetwork load_yaml_file(std::filesystem::path path) {
  std::ifstream saved_file(path.string());
  std::string content((std::istreambuf_iterator<char>(saved_file)), std::istreambuf_iterator<char>());
  std::vector<std::shared_ptr<AbstractNode>> nodes;
  YAML::Node output = YAML::Load(content);

  auto network = deserialize_network(output);

  return network;
}
};  // namespace NED
