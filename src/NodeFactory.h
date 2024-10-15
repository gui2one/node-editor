#ifndef NODE_EDITOR_NODEFACTORY_H
#define NODE_EDITOR_NODEFACTORY_H
#pragma once

#include <unordered_map>
#include "ImGuiNode.h"

namespace NodeEditor {
struct NodeFactoryRegistryItem{
    std::string category_name;
    std::function<std::shared_ptr<ImGuiNode>()> factory_func;
};
// Registry to hold factories
class NodeFactoryRegistry {
public:
    using FactoryFunc = std::function<std::shared_ptr<ImGuiNode>()>;

    static NodeFactoryRegistry& instance() {
        static NodeFactoryRegistry registry;
        return registry;
    }

    void registerType(const std::string& typeName, NodeFactoryRegistryItem item) {
        factories[typeName] = item;
    }

    std::shared_ptr<ImGuiNode> create(const std::string& typeName) const {
        auto it = factories.find(typeName);
        if (it != factories.end()) {
            return it->second.factory_func();
        }
        return nullptr; // Type not found
    }

private:
    std::unordered_map<std::string, NodeFactoryRegistryItem> factories;
};
};

#define REGISTER_NODE_TYPE(Category, Type) \
    NodeEditor::NodeFactoryRegistry::instance().registerType(#Type, { \
        Category,\
    []() -> std::shared_ptr<NodeEditor::Node<Type>> { return std::make_shared<NodeEditor::Node<Type>>("factory_node"); }\
    })

#endif // NODE_EDITOR_NODEFACTORY_H