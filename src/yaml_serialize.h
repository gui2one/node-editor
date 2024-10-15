#ifndef NODE_EDITOR_YAML_SERIALIZE_H
#define NODE_EDITOR_YAML_SERIALIZE_H

#include <memory>
#include <yaml-cpp/yaml.h>
#include "ImGuiNode.h"
#include "NodeParam.h"
#include "NodeFactory.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm/glm.hpp>

#include "utils/node_manager_utils.h"

#define YAML_CONVERT(type_str) \
  as<type_str>()

#define SET_PARAM_VALUE(param, value, type)\
   set_param_value(param, value.YAML_CONVERT(type))

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
struct convert<std::shared_ptr<NodeEditor::ImGuiNode>> {
  static Node encode(const std::shared_ptr<NodeEditor::ImGuiNode>& rhs) {
    Node node;
    node["title"] = rhs->title;
    node["position"] = rhs->position;
    node["size"] = rhs->size;
    return node;
  }

  static bool decode(const Node& node, std::shared_ptr<NodeEditor::ImGuiNode>& rhs) {
    if(!node.IsMap() || node.size() != 3) {
      return false;
    }

    rhs->title = node["title"].as<std::string>();
    rhs->position = node["position"].as<ImVec2>();
    rhs->size = node["size"].as<ImVec2>();
    return true;
  }
};

template<>
struct convert<std::shared_ptr<NodeEditor::NodeParam>> {
  static Node encode(const std::shared_ptr<NodeEditor::NodeParam>& rhs) {
    Node node;
    node["name"] = std::string(rhs->name);
    // node["value"] = rhs->Eval();
    return node;
  }

  static bool decode(const Node& node, std::shared_ptr<NodeEditor::NodeParam>& rhs) {
    if(!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs->name = node["name"].as<std::string>().c_str();  
    // rhs->value = node["value"];
    return true;
  }
};

template<>
struct convert<NodeEditor::ParamLayoutItem> {
  static Node encode(const NodeEditor::ParamLayoutItem& rhs) {
    Node node;
    node["name"] = rhs.name;
    node["param"] = rhs.param;
    return node;
  }

  static bool decode(const Node& node, NodeEditor::ParamLayoutItem& rhs) {
    if(!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs.name = node["name"].as<std::string>();  
    rhs.param = node["param"].as<std::shared_ptr<NodeEditor::NodeParam>>();
    return true;
  }
};
}//END namespace YAML

YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v);
YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::ImGuiNode>& node);



std::string serialize_nodes(std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes);

std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> deserialize_nodes(std::string yaml);
#endif // NODE_EDITOR_YAML_SERIALIZE_H