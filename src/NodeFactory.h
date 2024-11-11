#ifndef NODE_EDITOR_NODEFACTORY_H
#define NODE_EDITOR_NODEFACTORY_H
#pragma once

#include <unordered_map>

#include "ImGuiNode.h"
#include "ParamFactory.h"

namespace NodeEditor {
struct NodeFactoryRegistryItem {
  std::string category_name;
  std::string label;
  std::string type_name;
  std::function<std::shared_ptr<AbstractNode>()> factory_func;
};
// Registry to hold factories
class NodeFactoryRegistry {
 public:
  using FactoryFunc = std::function<std::shared_ptr<AbstractNode>()>;

  static NodeFactoryRegistry& GetInstance();

  inline void RegisterType(const std::string& typeName, NodeFactoryRegistryItem item) { factories[typeName] = item; }

  std::shared_ptr<AbstractNode> Create(const std::string& typeName, AbstractNode* parent_node = nullptr) const;
  std::shared_ptr<AbstractNode> Clone(std::shared_ptr<AbstractNode> other) const;
  void CloneParam(std::shared_ptr<NodeParam> src, std::shared_ptr<NodeParam> dst) const;


  inline std::unordered_map<std::string, NodeFactoryRegistryItem> GetFactories() const { return factories; }

 private:
  std::unordered_map<std::string, NodeFactoryRegistryItem> factories;
};
};  // namespace NodeEditor

#define REGISTER_NODE_TYPE(Type, Label, Category)                                       \
  NodeEditor::NodeFactoryRegistry::GetInstance().RegisterType(                             \
      #Type, {Category, Label, #Type, []() -> std::shared_ptr<NodeEditor::Node<Type>> { \
                return std::make_shared<NodeEditor::Node<Type>>(Label);                 \
              }})
#define STRINGIFY(x) #x
#define CREATE_SUBNET_INPUT_NODE_CLASS(Type, Label, Category)                                 \
  NodeEditor::NodeFactoryRegistry::GetInstance().RegisterType(                                   \
      "NodeEditor::SubnetInputNode<" STRINGIFY(Type) ">",                                     \
      {Category, Label, "NodeEditor::SubnetInputNode<" STRINGIFY(Type) ">",                   \
       []() -> std::shared_ptr<NodeEditor::Node<NodeEditor::SubnetInputNode<Type>>> {         \
         return std::make_shared<NodeEditor::Node<NodeEditor::SubnetInputNode<Type>>>(Label); \
       }})
#endif  // NODE_EDITOR_NODEFACTORY_H