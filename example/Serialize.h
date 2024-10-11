#ifndef SERIALIZE_H
#define SERIALIZE_H

#pragma once

#include <yaml-cpp/yaml.h>
#include "StringGenerator.h"
#include "params.h"

YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v);
YAML::Emitter& operator << (YAML::Emitter& out, const ImVec2& v);
YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NodeEditor::ImGuiNode>& node);

template<typename T>
YAML::Emitter& operator << (YAML::Emitter& out, NodeEditor::Param<T>* param) {
	out << YAML::Key <<param->name;
  out << param->Eval();
	return out;
}


void serialize_nodes(std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes);

#endif // SERIALIZE_H