#include "NodeFactory.h"

namespace NodeEditor {
NodeFactoryRegistry& NodeEditor::NodeFactoryRegistry::instance() {
  
  static NodeFactoryRegistry registry;
  return registry;

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

  for(size_t i = 0; i < other->m_ParamLayout.items.size(); i++) {
    auto param = other->m_ParamLayout.items[i].param;
    std::string clean_name = clean_param_type_name(typeid(*param).name());
    std::cout << "Cloning param: " << clean_name << std::endl;
    auto factory_param = ParamFactoryRegistry::instance().create(clean_name);
    if(factory_param != nullptr) {
      
      std::cout << "\tfactory param: " << clean_param_type_name(typeid(*factory_param).name()) << "" << std::endl;
    }else{
      std::cout << "problem with: " << clean_param_type_name(typeid(*param).name()) << "" << std::endl;
      
    }
    
    
  }
  return factory_node;
}
};  // namespace NodeEditor
