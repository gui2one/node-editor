#include "NodeFactory.h"

namespace NodeEditor {
NodeFactoryRegistry& NodeEditor::NodeFactoryRegistry::instance() {
  {
    static NodeFactoryRegistry registry;
    return registry;
  }
}
std::shared_ptr<AbstractNode> NodeFactoryRegistry::create(const std::string& typeName) const {
  auto it = factories.find(typeName);
  if (it != factories.end()) {
    return it->second.factory_func();
  }
  return nullptr;  // Type not found
}
std::shared_ptr<AbstractNode> NodeFactoryRegistry::clone(std::shared_ptr<AbstractNode> other) const {
  auto type_str = typeid(*other.get()).name();
  auto clean_name = clean_node_type_name(type_str);
  auto factory_node = NodeFactoryRegistry::instance().create(clean_name);  
  factory_node->title = other->title + "_clone";
  factory_node->position = other->position + ImVec2(10, 10);
  return factory_node;
}
};  // namespace NodeEditor
