#ifndef NODE_EDITOR_YAML_CONVERT_H
#define NODE_EDITOR_YAML_CONVERT_H
#pragma once

#include <imgui.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>

#include "ImGuiNode.h"
#include "utils.h"

namespace YAML {

template <>
struct convert<std::wstring> {
  static Node encode(const std::wstring& rhs) {
    // std::cout << "Encoding Wstring to yaml" << std::endl;

    Node node;
    node = NED::wide_to_utf8(rhs);

    return node;
  }

  static bool decode(const Node& node, std::wstring& rhs) {
    // std::cout << "Decoding Wstring from yaml" << std::endl;

    if (!node || !node.IsScalar()) {
      return false;
    }
    auto utf8 = node.as<std::string>();
    rhs = NED::utf8_to_wide(utf8);
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
struct convert<std::shared_ptr<NED::AbstractNode>> {
  static Node encode(const std::shared_ptr<NED::AbstractNode>& rhs) {
    Node node;
    node["title"] = rhs->title;
    node["position"] = rhs->position;
    node["size"] = rhs->size;
    return node;
  }

  static bool decode(const Node& node, std::shared_ptr<NED::AbstractNode>& rhs) {
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
struct convert<std::shared_ptr<NED::NodeParam>> {
  static Node encode(const std::shared_ptr<NED::NodeParam>& rhs) {
    Node node;
    node["name"] = std::string(rhs->m_Label);
    // node["value"] = rhs->Eval();
    return node;
  }

  static bool decode(const Node& node, std::shared_ptr<NED::NodeParam>& rhs) {
    if (!node.IsMap() || node.size() != 2) {
      return false;
    }

    rhs->m_Label = node["name"].as<std::string>().c_str();
    // rhs->value = node["value"];
    return true;
  }
};

}  // END namespace YAML

#endif  // NODE_EDITOR_YAML_CONVERT_H