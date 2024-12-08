#include "NodeFactory.h"

namespace NED {
NodeFactoryRegistry& NED::NodeFactoryRegistry::GetInstance() {
  static NodeFactoryRegistry registry;
  return registry;
}
std::shared_ptr<AbstractNode> NodeFactoryRegistry::Create(const std::string& typeName,
                                                          AbstractNode* parent_node) const {
  auto it = factories.find(typeName);
  if (it != factories.end()) {
    return it->second.factory_func();
  }

  return nullptr;  // Type not found
}
std::shared_ptr<AbstractNode> NodeFactoryRegistry::Clone(std::shared_ptr<AbstractNode> other, bool set_new_position,
                                                         ImVec2 new_pos) const {
  auto factory_node = NodeFactoryRegistry::GetInstance().Create(other->m_TypeName);
  factory_node->title = other->title;
  if (set_new_position) {
    factory_node->position = new_pos;
  } else {
    factory_node->position = other->position + ImVec2(10, 10);
  }
  factory_node->parent_node = other->parent_node;

  // clone inputs
  for (size_t i = 0; i < MAX_N_INPUTS; i++) {
    factory_node->inputs[i] = other->inputs[i];
  }

  // clone MiultiInputs
  factory_node->m_MultiInput = other->m_MultiInput;

  for (size_t i = 0; i < other->m_ParamLayout.params.size(); i++) {
    auto param = other->m_ParamLayout.params[i];

    auto factory_param = factory_node->m_ParamLayout.params[i];
    CloneParam(param, factory_param);
  }

  // TODO : clone subnet inner Nodes with connections and all
  if (factory_node->IsSubnet()) {
    for (auto child : other->node_network.nodes) {
      auto factory_child = NodeFactoryRegistry::GetInstance().Clone(child);
      factory_node->node_network.AddNode(factory_child);
    }
  }
  return factory_node;
}
void NodeFactoryRegistry::CloneParam(std::shared_ptr<NodeParam> src_param, std::shared_ptr<NodeParam> dst_param) const {
  if (src_param != nullptr) {
    auto p_label = std::dynamic_pointer_cast<NED::ParamLabel>(src_param);
    auto p_group = std::dynamic_pointer_cast<NED::ParamGroup>(src_param);
    auto p_file = std::dynamic_pointer_cast<NED::ParamFile>(src_param);
    auto p_int = std::dynamic_pointer_cast<NED::ParamInt>(src_param);
    auto p_float = std::dynamic_pointer_cast<NED::ParamFloat>(src_param);
    auto p_string = std::dynamic_pointer_cast<NED::Param<std::string>>(src_param);
    auto p_bool = std::dynamic_pointer_cast<NED::Param<bool>>(src_param);
    auto p_vec2 = std::dynamic_pointer_cast<NED::ParamVec2>(src_param);
    auto p_vec3 = std::dynamic_pointer_cast<NED::ParamVec3>(src_param);
    if (p_label != nullptr) {
      auto factory_p_label = std::dynamic_pointer_cast<NED::ParamLabel>(dst_param);
      factory_p_label->value = p_label->value;
    } else if (p_group != nullptr) {
      auto factory_p_group = std::dynamic_pointer_cast<NED::ParamGroup>(dst_param);
      factory_p_group->value = p_group->value;
      for (size_t j = 0; j < p_group->params.size(); j++) {
        CloneParam(p_group->params[j], factory_p_group->params[j]);
      }
      // std::cout << "Duplicating ParamGroup: " << std::endl;

    } else if (p_file != nullptr) {
      // std::cout << "Duplicating ParamFile: " << "" << std::endl;
      auto factory_p_file = std::dynamic_pointer_cast<NED::ParamFile>(dst_param);
      factory_p_file->value = p_file->value;

    } else if (p_int != nullptr) {
      auto factory_p_int = std::dynamic_pointer_cast<NED::ParamInt>(dst_param);
      factory_p_int->value = p_int->value;
    } else if (p_float != nullptr) {
      auto factory_p_float = std::dynamic_pointer_cast<NED::ParamFloat>(dst_param);
      factory_p_float->value = p_float->value;
    } else if (p_string != nullptr) {
      auto factory_p_string = std::dynamic_pointer_cast<NED::Param<std::string>>(dst_param);
      factory_p_string->value = p_string->value;
    } else if (p_bool != nullptr) {
      auto factory_p_bool = std::dynamic_pointer_cast<NED::Param<bool>>(dst_param);
      factory_p_bool->value = p_bool->value;
    } else if (p_vec2 != nullptr) {
      auto factory_p_vec2 = std::dynamic_pointer_cast<NED::Param<glm::vec2>>(dst_param);
      factory_p_vec2->value = p_vec2->value;
    } else if (p_vec3 != nullptr) {
      auto factory_p_vec3 = std::dynamic_pointer_cast<NED::Param<glm::vec3>>(dst_param);
      factory_p_vec3->value = p_vec3->value;
    }
  } else {
    std::cout << "problem with: " << dst_param->m_TypeName << "" << std::endl;
  }
}
};  // namespace NED
