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

static std::shared_ptr<AbstractNode> find_node_by_title(NodeNetwork* network, std::string title) {
  for (auto node : network->nodes) {
    if (node->title == title) {
      return node;
    }
  }

  return nullptr;
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

  // clone MultiInputs
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

    /*
     * here I update inputs in case of a subnet being cloned. ( because the network 'context' changes )
     * the strategy is to use node titles, which should be unique per node_nework, but if we clone a subnet, children
     * titles should remain the same
     */
    if (factory_node->IsSubnet()) {
      for (auto child : factory_node->node_network.nodes) {
        for (size_t i = 0; i < MAX_N_INPUTS; i++) {
          if (child->inputs[i] != nullptr) {
            auto _title = child->inputs[i]->title;

            // std::cout << "input title : " << _title << std::endl;
            auto new_parent = find_node_by_title(&factory_node->node_network, _title);
            if (new_parent != nullptr) {
              // std::cout << "Found new parent ?!" << std::endl;
              child->inputs[i] = new_parent.get();
            }
          }
        }
      }
    }
  }

  return factory_node;
}
void NodeFactoryRegistry::CloneParam(std::shared_ptr<NodeParam> src_param, std::shared_ptr<NodeParam> dst_param) const {
  if (src_param != nullptr) {
    auto group_p = std::dynamic_pointer_cast<ParamGroup>(src_param);
    if (group_p != nullptr) {
      auto group_p_dst = std::dynamic_pointer_cast<ParamGroup>(dst_param);
      for (size_t i = 0; i < group_p->params.size(); i++) {
        CloneParam(group_p->params[i], group_p_dst->params[i]);
      }
    } else {
      src_param->Clone(dst_param);
    }

  } else {
    std::cout << "problem with: " << dst_param->m_TypeName << "" << std::endl;
  }
}
};  // namespace NED
