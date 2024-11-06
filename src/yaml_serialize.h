#ifndef NODE_EDITOR_YAML_SERIALIZE_H
#define NODE_EDITOR_YAML_SERIALIZE_H
#pragma once

#include <memory>
#include <fstream>
#include "NodeParam.h"
#include <yaml-cpp/yaml.h>
#include "ImGuiNode.h"
#include "NodeFactory.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "utils/node_manager_utils.h"

// #include "yaml_convert.h"

YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::AbstractNode>& node);
namespace NodeEditor {

//forward declaration
struct NodeNetwork;

YAML::Node serialize_network(NodeNetwork network);
YAML::Node serialize_nodes(std::vector<std::shared_ptr<AbstractNode>> nodes);
void save_all(std::filesystem::path path, NodeNetwork& network);

std::shared_ptr<NodeParam> deserialize_param(YAML::Node yaml, std::shared_ptr<AbstractNode> factory_node);
NodeNetwork deserialize_network(YAML::Node yaml);
std::vector<std::shared_ptr<AbstractNode>> deserialize_nodes(YAML::Node yaml);
NodeNetwork load_yaml_file(std::filesystem::path path);

};
#endif // NODE_EDITOR_YAML_SERIALIZE_H