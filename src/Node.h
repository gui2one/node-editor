#ifndef NODE_EDITOR_NODE_H
#define NODE_EDITOR_NODE_H
#pragma once
#include "ImGuiNode.h"
#include "constants.h"
namespace NED {

// forward declaration
class AbstractNode;

template <typename T>
class Node : public T {
 public:
  Node(const char *_title, const char *_type_name) {
    auto node = static_cast<AbstractNode *>(this);
    node->title = _title;
    node->m_TypeName = _type_name;
  }

  void Update() {
    auto node = static_cast<AbstractNode *>(this);
    node->m_IsCooking = true;

    std::string hash = node->GenerateParamsHash();
    // std::cout << "HASHH : " << hash << std::endl;
    node->m_ParamsHash = hash;

    auto op = dynamic_cast<T *>(this);
    if (!node->IsMultiInput()) {
      if (node->IsSubnetInputNode()) {
        if (node->parent_node != nullptr) {
          for (uint32_t i = 0; i < node->parent_node->GetNumAvailableInputs(); i++) {
            if (node->parent_node->GetInput(i) != nullptr) {
              node->parent_node->GetInput(i)->Update();
            }
          }
        }
      } else {
        for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
          if (node->GetInput(i) != nullptr) {
            node->GetInput(i)->Update(); /* Important !!*/
          }
        }
      }
    } else {
      for (uint32_t i = 0; i < node->GetMultiInputCount(); i++) {
        if (node->GetMultiInput(i).node != nullptr) {
          node->GetMultiInput(i).node->Update(); /* Important !!*/
        }
      }
    }

    if (node->IsSubnet()) {
      for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
        if (node->GetInput(i) != nullptr) {
          node->GetInput(i)->Update();
        }
      }
      if (node->node_network.outuput_node != nullptr) {
        node->node_network.outuput_node->Update();

      } else {
        std::cout << "Subnet has no ouput Node" << std::endl;
      }
    }
    op->Generate();

    node->m_IsCooking = false;
  }

  T *ToOperator() { return static_cast<T *>(this); }
};
};  // namespace NED
#endif  // NODE_EDITOR_NODE_H