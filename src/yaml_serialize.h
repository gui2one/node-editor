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
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    return node;
  }

  static bool decode(const Node& node, ImVec2& rhs) {
    if(!node.IsSequence() || node.size() != 2) {
      return false;
    }

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    return true;
  }
};
}



YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v);
YAML::Emitter& operator << (YAML::Emitter& out, const ImVec2& v);
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