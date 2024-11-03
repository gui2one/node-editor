#ifndef NODE_EDITOR_NODE_H
#define NODE_EDITOR_NODE_H
#pragma once
#include "ImGuiNode.h"
#include "constants.h"
namespace NodeEditor {

//forward declaration
class AbstractNode;
class AbstractSubnetInputNode;

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
    if(!node->IsMultiInput()) {
        if(node->IsSubnetInputNode()) {
            
            if(node->parent_node->GetInput(0) != nullptr){
                std::cout << "Trying to update a Subnet Input Node" << std::endl;
                node->parent_node->GetInput(0)->Update();
            }
            // node->parent_node->Generate();
        }else{  

            for (uint32_t i = 0; i < MAX_N_INPUTS; i++) {
                if (node->GetInput(i) != nullptr) {
                    auto inp = node->GetInput(i);
                    if(inp->IsSubnet()){
                        if(inp->node_network.outuput_node != nullptr){
                            inp->node_network.outuput_node->Update();
                        }
                    }else{

                        inp->Update(); /* Important !!*/
                    }
                }
            }
        }
    }else{
        for (uint32_t i = 0; i < node->GetMultiInputCount(); i++) {
            if (node->GetMultiInput(i) != nullptr) {
                node->GetMultiInput(i)->Update(); /* Important !!*/
            }
        }

    }

    if(node->IsSubnet()) {

        if( node->node_network.outuput_node != nullptr){

            node->node_network.outuput_node->Update();

        }else{
            std::cout << "Subnet has no ouput Node" << std::endl;
        }
    }
    op->Generate();
  }

  T *ToOperator() { return static_cast<T *>(this); }
};
};
#endif // NODE_EDITOR_NODE_H