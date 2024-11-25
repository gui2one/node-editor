#ifndef NODE_EDITOR_NODEFACTORY_H
#define NODE_EDITOR_NODEFACTORY_H
#pragma once

#include <unordered_map>

#include "ImGuiNode.h"
#include "ParamFactory.h"

namespace NED {
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
};  // namespace NED

#define REGISTER_NODE_TYPE(Type, Label, Category)       \
  NED::NodeFactoryRegistry::GetInstance().RegisterType( \
      #Type, {Category, Label, #Type,                   \
              []() -> std::shared_ptr<NED::Node<Type>> { return std::make_shared<NED::Node<Type>>(Label, #Type); }})
#define CREATE_SUBNET_INPUT_NODE_CLASS(Type, Label, Category)                                               \
  NED::NodeFactoryRegistry::GetInstance().RegisterType(                                                     \
      "NED::SubnetInputNode<"#Type">", {Category, Label, "NED::SubnetInputNode<"#Type">",               \
                                          []() -> std::shared_ptr<NED::Node<NED::SubnetInputNode<Type>>> {  \
                                            return std::make_shared<NED::Node<NED::SubnetInputNode<Type>>>( \
                                                Label, "NED::SubnetInputNode<"#Type">");                  \
                                          }})
#define CREATE_UTILITY_CLASSES(Type, Category)                                               \
  NED::NodeFactoryRegistry::GetInstance().RegisterType(                                                     \
      "NED::SubnetInputNode<"#Type">", {Category, "Subnet Input", "NED::SubnetInputNode<"#Type">",               \
                                          []() -> std::shared_ptr<NED::Node<NED::SubnetInputNode<Type>>> {  \
                                            return std::make_shared<NED::Node<NED::SubnetInputNode<Type>>>( \
                                                "Subnet Input", "NED::SubnetInputNode<"#Type">");                  \
                                          }}); \
  NED::NodeFactoryRegistry::GetInstance().RegisterType(                                                     \
      "NED::SubnetNode<"#Type">", {Category, "Subnet", "NED::SubnetNode<"#Type">",               \
                                          []() -> std::shared_ptr<NED::Node<NED::SubnetNode<Type>>> {  \
                                            return std::make_shared<NED::Node<NED::SubnetNode<Type>>>( \
                                                "Subnet", "NED::SubnetNode<"#Type">");                  \
                                          }}); \
  NED::NodeFactoryRegistry::GetInstance().RegisterType(                                                     \
      "NED::NullNode<"#Type">", {Category, "Null", "NED::NullNode<"#Type">",               \
                                          []() -> std::shared_ptr<NED::Node<NED::NullNode<Type>>> {  \
                                            return std::make_shared<NED::Node<NED::NullNode<Type>>>( \
                                                "Null", "NED::NullNode<"#Type">");                  \
                                          }})




#endif  // NODE_EDITOR_NODEFACTORY_H