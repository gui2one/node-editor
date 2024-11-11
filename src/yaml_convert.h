#ifndef NODE_EDITOR_YAML_CONVERT_H
#define NODE_EDITOR_YAML_CONVERT_H
#pragma once

#include <imgui.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>

#include "ImGuiNode.h"


namespace YAML {

template <>
struct convert<std::wstring> {
  static Node encode(const std::wstring& rhs) {
    // std::cout << "Encoding Wstring to yaml" << std::endl;

    Node node;
    node["value"] = rhs;

    return node;
  }

  static bool decode(const Node& node, std::wstring& rhs) {
    rhs = node["value"].as<std::wstring>();
    return true;
  }
};

template <>
struct convert<ImVec2> {
  static Node encode(const ImVec2& rhs) {
    Node node;
    node["x"] = rhs.x;
    node["y"] = rhs.y;
    return node;
  }

  static bool decode(const Node& node, ImVec2& rhs) {
    if (!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs.x = node["x"].as<float>();
    rhs.y = node["y"].as<float>();
    return true;
  }
};

template <>
struct convert<glm::vec2> {
  static Node encode(const glm::vec2& rhs) {
    Node node;
    node["x"] = rhs.x;
    node["y"] = rhs.y;
    return node;
  }

  static bool decode(const Node& node, glm::vec2& rhs) {
    if (!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs.x = node["x"].as<float>();
    rhs.y = node["y"].as<float>();
    return true;
  }
};

template <>
struct convert<glm::vec3> {
  static Node encode(const glm::vec3& rhs) {
    Node node;
    node["x"] = rhs.x;
    node["y"] = rhs.y;
    node["z"] = rhs.z;
    return node;
  }

  static bool decode(const Node& node, glm::vec3& rhs) {
    if (!node.IsMap() || node.size() != 3) {
      return false;
    }

    rhs.x = node["x"].as<float>();
    rhs.y = node["y"].as<float>();
    rhs.z = node["z"].as<float>();
    return true;
  }
};

template <>
struct convert<std::shared_ptr<NodeEditor::AbstractNode>> {
  static Node encode(const std::shared_ptr<NodeEditor::AbstractNode>& rhs) {
    Node node;
    node["title"] = rhs->title;
    node["position"] = rhs->position;
    node["size"] = rhs->size;
    return node;
  }

  static bool decode(const Node& node, std::shared_ptr<NodeEditor::AbstractNode>& rhs) {
    if (!node.IsMap() || node.size() != 3) {
      return false;
    }

    rhs->title = node["title"].as<std::string>();
    rhs->position = node["position"].as<ImVec2>();
    rhs->size = node["size"].as<ImVec2>();
    return true;
  }
};

template <>
struct convert<std::shared_ptr<NodeEditor::NodeParam>> {
  static Node encode(const std::shared_ptr<NodeEditor::NodeParam>& rhs) {
    Node node;
    node["name"] = std::string(rhs->name);
    // node["value"] = rhs->Eval();
    return node;
  }

  static bool decode(const Node& node, std::shared_ptr<NodeEditor::NodeParam>& rhs) {
    if (!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs->name = node["name"].as<std::string>().c_str();
    // rhs->value = node["value"];
    return true;
  }
};

// template <>
// struct convert<NodeEditor::ParamLayoutItem> {
//   static Node encode(const NodeEditor::ParamLayoutItem& rhs) {
//     Node node;
//     node["name"] = rhs.name;
//     node["param"] = rhs.param;
//     return node;
//   }

//   static bool decode(const Node& node, NodeEditor::ParamLayoutItem& rhs) {
//     if (!node.IsMap() || node.size() != 2) {
//       return false;
//     }

//     rhs.name = node["name"].as<std::string>();
//     rhs.param = node["param"].as<std::shared_ptr<NodeEditor::NodeParam>>();
//     return true;
//   }
// };
}  // END namespace YAML

#endif  // NODE_EDITOR_YAML_CONVERT_H