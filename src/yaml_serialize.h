#ifndef NODE_EDITOR_YAML_SERIALIZE_H
#define NODE_EDITOR_YAML_SERIALIZE_H

#include <memory>
#include "NodeParam.h"
#include <yaml-cpp/yaml.h>
#include "ImGuiNode.h"
#include "NodeFactory.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "utils/node_manager_utils.h"

YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::ImGuiNode>& node);

std::string serialize_nodes(std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes);

std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> deserialize_nodes(std::string yaml);
#endif // NODE_EDITOR_YAML_SERIALIZE_H