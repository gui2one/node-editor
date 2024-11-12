#ifndef NODE_EDITOR_NODE_H
#define NODE_EDITOR_NODE_H
#pragma once
#include "ImGuiNode.h"
#include "constants.h"
namespace NodeEditor {

// forward declaration
class AbstractNode;

template <typename T>
class Node : public T {
 public:
  Node(const char *_title) {
    auto node = static_cast<AbstractNode *>(this);
    node->title = _title;
  }

  void Update() {
    auto node = static_cast<AbstractNode *>(this);
    auto op = static_cast<T *>(this);
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
        if (node->GetMultiInput(i) != nullptr) {
          node->GetMultiInput(i)->Update(); /* Important !!*/
        }
      }
    }

    if (node->IsSubnet()) {
      if (node->node_network.outuput_node != nullptr) {
        node->node_network.outuput_node->Update();

      } else {
        std::cout << "Subnet has no ouput Node" << std::endl;
      }

      for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
        if (node->GetInput(i) != nullptr) {
          node->GetInput(i)->Update();
        }
      }
    }
    op->Generate();
  }

  T *ToOperator() { return static_cast<T *>(this); }
};
};  // namespace NodeEditor
#endif  // NODE_EDITOR_NODE_H