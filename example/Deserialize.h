#ifndef EXAMPLE_DESERIALIZE_H
#define EXAMPLE_DESERIALIZE_H
#pragma once

#include <yaml-cpp/yaml.h>
#include "StringGenerator.h" // our 'string builder' classes
#include "params.h"
#include "utils.h"
#include "ImGuiNode.h"
#include <fstream>

std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> deserialize_yaml_save(const char* filename);

template<typename T>
std::shared_ptr<NodeEditor::Node<T>> basic_node_convert(const YAML::Node& node) {
    auto title = node["title"].as<std::string>();
    auto uuid = node["uuid"].as<std::string>();
    auto position = node["position"].as<std::vector<float>>();

    auto op = std::make_shared<NodeEditor::Node<T>>("default name");
    op->title = std::string(title);
    op->uuid = uuid;
    op->position = ImVec2(position[0], position[1]);
    for(size_t i = 0; i < node["params"].size(); i++) {
        auto p = node["params"][i];

        std::string type_str = std::string(p["type"].as<std::string>());
        if( type_str.find("class std::basic_string") != std::string::npos) {
            
            auto val = std::string(p["value"].as<std::string>());
            set_param_value<std::string>(op->m_Params[0], val);
        
        }else if(type_str == "class NodeEditor::Param<unsigned int>") {
        
            auto val = p["value"].as<uint32_t>();
            set_param_value<uint32_t>(op->m_Params[0], val);
        }
        
    }
    return op;
}

template<typename T>
std::shared_ptr<NodeEditor::Param<T>> param_convert(const YAML::Node& node) {
    auto result = std::make_shared<NodeEditor::Param<T>>("default param name");

    return result;
}
#endif // EXAMPLE_DESERIALIZE_H