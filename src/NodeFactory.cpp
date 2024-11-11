#include "NodeFactory.h"

namespace NodeEditor {
NodeFactoryRegistry& NodeEditor::NodeFactoryRegistry::GetInstance() {
  static NodeFactoryRegistry registry;
  return registry;
}
std::shared_ptr<AbstractNode> NodeFactoryRegistry::Create(const std::string& typeName, AbstractNode* parent_node) const {
  auto it = factories.find(typeName);
  if (it != factories.end()) {
    return it->second.factory_func();
  }

  return nullptr;  // Type not found
}
std::shared_ptr<AbstractNode> NodeFactoryRegistry::Clone(std::shared_ptr<AbstractNode> other) const {
  auto type_str = typeid(*other.get()).name();
  auto clean_name = clean_node_type_name(type_str);
  auto factory_node = NodeFactoryRegistry::GetInstance().Create(clean_name);
  factory_node->title = other->title + "_clone";
  factory_node->position = other->position + ImVec2(10, 10);
  factory_node->parent_node = other->parent_node;

  // clone inputs
  for(size_t i = 0; i < MAX_N_INPUTS; i++) {
    factory_node->inputs[i] = other->inputs[i];
  }

  // clone MiultiInputs
  factory_node->m_MultiInput = other->m_MultiInput;

  for (size_t i = 0; i < other->m_ParamLayout.params.size(); i++) {
    auto param = other->m_ParamLayout.params[i];
    std::string clean_name = clean_param_type_name(typeid(*param).name());
    // std::cout << "Cloning param: " << clean_name << std::endl;
    auto factory_param = factory_node->m_ParamLayout.params[i];
    CloneParam(param, factory_param);
   
  }
  return factory_node;
}
void NodeFactoryRegistry::CloneParam(std::shared_ptr<NodeParam> src_param, std::shared_ptr<NodeParam> dst_param) const {
  if (src_param != nullptr) {
      auto p_label = std::dynamic_pointer_cast<NodeEditor::ParamLabel>(src_param);
      auto p_group = std::dynamic_pointer_cast<NodeEditor::ParamGroup>(src_param);
      auto p_file = std::dynamic_pointer_cast<NodeEditor::ParamFile>(src_param);
      auto p_int = std::dynamic_pointer_cast<NodeEditor::Param<int>>(src_param);
      auto p_float = std::dynamic_pointer_cast<NodeEditor::Param<float>>(src_param);
      auto p_string = std::dynamic_pointer_cast<NodeEditor::Param<std::string>>(src_param);
      auto p_bool = std::dynamic_pointer_cast<NodeEditor::Param<bool>>(src_param);
      auto p_vec2 = std::dynamic_pointer_cast<NodeEditor::Param<glm::vec2>>(src_param);
      auto p_vec3 = std::dynamic_pointer_cast<NodeEditor::Param<glm::vec3>>(src_param);
      if (p_label != nullptr) {
        auto factory_p_label = std::dynamic_pointer_cast<NodeEditor::ParamLabel>(dst_param);
        factory_p_label->value = p_label->value;
        std::cout << "Duplicating ParamLabel: " << std::endl;

      } else if (p_group != nullptr) {
        auto factory_p_group = std::dynamic_pointer_cast<NodeEditor::ParamGroup>(dst_param);
        factory_p_group->value = p_group->value;
        for (size_t j = 0; j < p_group->items.size(); j++) {
          std::cout << "WORK TODO HERE" << std::endl;
          CloneParam(p_group->items[j], factory_p_group->items[j]);
        }
        std::cout << "Duplicating ParamGroup: " << std::endl;

      } else if (p_file != nullptr) {
        std::cout << "Duplicating ParamFile: " << "" << std::endl;
        auto factory_p_file = std::dynamic_pointer_cast<NodeEditor::ParamFile>(dst_param);
        factory_p_file->value = p_file->value;

      } else if (p_int != nullptr) {
        auto factory_p_int = std::dynamic_pointer_cast<NodeEditor::Param<int>>(dst_param);
        factory_p_int->value = p_int->value;
      } else if (p_float != nullptr) {
        auto factory_p_float = std::dynamic_pointer_cast<NodeEditor::Param<float>>(dst_param);
        factory_p_float->value = p_float->value;
      } else if (p_string != nullptr) {
        auto factory_p_string = std::dynamic_pointer_cast<NodeEditor::Param<std::string>>(dst_param);
        factory_p_string->value = p_string->value;
      } else if (p_bool != nullptr) {
        auto factory_p_bool = std::dynamic_pointer_cast<NodeEditor::Param<bool>>(dst_param);
        factory_p_bool->value = p_bool->value;
      } else if (p_vec2 != nullptr) {
        auto factory_p_vec2 = std::dynamic_pointer_cast<NodeEditor::Param<glm::vec2>>(dst_param);
        factory_p_vec2->value = p_vec2->value;
      } else if (p_vec3 != nullptr) {
        auto factory_p_vec3 = std::dynamic_pointer_cast<NodeEditor::Param<glm::vec3>>(dst_param);
        factory_p_vec3->value = p_vec3->value;
      }
      // std::cout << "\tfactory param: " << clean_param_type_name(typeid(*dst_param).name()) << "" << std::endl;
    } else {
      std::cout << "problem with: " << clean_param_type_name(typeid(*dst_param).name()) << "" << std::endl;
    }
}
};  // namespace NodeEditor
