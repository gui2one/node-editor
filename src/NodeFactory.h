#ifndef NODE_EDITOR_NODEFACTORY_H
#define NODE_EDITOR_NODEFACTORY_H
#pragma once

#include <unordered_map>
// #include <memory>
// #include <functional>
// #include <string>
#include "ImGuiNode.h"

namespace NodeEditor {
// Registry to hold factories
class NodeFactoryRegistry {
public:
    using FactoryFunc = std::function<std::shared_ptr<ImGuiNode>()>;

    static NodeFactoryRegistry& instance() {
        static NodeFactoryRegistry registry;
        return registry;
    }

    void registerType(const std::string& typeName, FactoryFunc factory) {
        factories[typeName] = factory;
    }

    std::shared_ptr<ImGuiNode> create(const std::string& typeName) const {
        auto it = factories.find(typeName);
        if (it != factories.end()) {
            return it->second();
        }
        return nullptr; // Type not found
    }

private:
    std::unordered_map<std::string, FactoryFunc> factories;
};
};

#define REGISTER_NODE_TYPE(Type) \
    NodeEditor::NodeFactoryRegistry::instance().registerType(#Type, []() -> std::shared_ptr<NodeEditor::Node<Type>> { return std::make_shared<NodeEditor::Node<Type>>("default_name"); })

#endif // NODE_EDITOR_NODEFACTORY_H