#ifndef NODE_EDITOR_YAML_SERIALIZE_H
#define NODE_EDITOR_YAML_SERIALIZE_H

#include <memory>
#include <yaml-cpp/yaml.h>
#include "ImGuiNode.h"
#include "NodeParam.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm/glm.hpp>

namespace YAML {
template<>
struct convert<ImVec2> {
  static Node encode(const ImVec2& rhs) {
    Node node;
    node["x"] = rhs.x;
    node["y"] = rhs.y;
    // node.push_back(rhs.x);
    // node.push_back(rhs.y);
    return node;
  }

  static bool decode(const Node& node, ImVec2& rhs) {
    if(!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs.x = node["x"].as<float>();
    rhs.y = node["y"].as<float>();
    return true;
  }
};

template<>
struct convert<NodeEditor::ImGuiNode> {
  static Node encode(const NodeEditor::ImGuiNode& rhs) {
    Node node;
    node["title"] = rhs.title;
    node["position"] = rhs.position;
    node["size"] = rhs.size;
    return node;
  }

  static bool decode(const Node& node, NodeEditor::ImGuiNode& rhs) {
    if(!node.IsMap() || node.size() != 3) {
      return false;
    }

    rhs.title = node["title"].as<std::string>();
    rhs.position = node["position"].as<ImVec2>();
    rhs.size = node["size"].as<ImVec2>();
    return true;
  }
};


};

YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v);
YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::NodeParam>& param);
YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::ImGuiNode>& node);

template<typename T>
YAML::Emitter& operator << (YAML::Emitter& out, NodeEditor::Param<T>* param) {
	out << YAML::Key <<param->name;
  out << param->Eval();
	return out;
}


std::string serialize_nodes(std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes);

#endif // NODE_EDITOR_YAML_SERIALIZE_H