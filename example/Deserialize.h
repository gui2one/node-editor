#ifndef EXAMPLE_DESERIALIZE_H
#define EXAMPLE_DESERIALIZE_H
#pragma once

#include <yaml-cpp/yaml.h>
#include "StringGenerator.h" // our 'string builder' classes
#include "params.h"
#include "ImGuiNode.h"
#include <fstream>

std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> deserialize_yaml_save(const char* filename);

template<typename T>
std::shared_ptr<NodeEditor::Node<T>> basic_convert(const YAML::Node& node) {
    auto title = node["title"].as<std::string>();
    auto uuid = node["uuid"].as<std::string>();
    auto position = node["position"].as<std::vector<float>>();

    auto op = std::make_shared<NodeEditor::Node<T>>("default name");
    op->title = std::string(title);
    op->uuid = uuid;
    op->position = ImVec2(position[0], position[1]);
    return op;
}
#endif // EXAMPLE_DESERIALIZE_H