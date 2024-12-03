#include "Action.h"

#include "NodeManager.h"
namespace NED {
MoveNodeAction::MoveNodeAction(AbstractNode* node, ImVec2 from_pos, ImVec2 to_pos)
    : from_pos(from_pos), to_pos(to_pos), m_Node(node) {
  message = std::format("Moved Node from {},{} to {},{}", from_pos.x, from_pos.y, to_pos.x, to_pos.y);
}

void MoveNodeAction::Do() { m_Node->position = to_pos; }
void MoveNodeAction::Undo() { m_Node->position = from_pos; }

NodeConnectAction::NodeConnectAction(AbstractNode* new_parent_node, AbstractNode* old_parent_node,
                                     AbstractNode* child_node, uint32_t child_input_index)
    : m_NewParentNode(new_parent_node),
      m_OldParentNode(old_parent_node),
      m_ChildNode(child_node),
      m_ChildInputIndex(child_input_index) {}

void NodeConnectAction::Do() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_NewParentNode);
  } else {
    m_ChildNode->AppendInput(m_NewParentNode);
  }
}

void NodeConnectAction::Undo() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_OldParentNode);
    if (m_OldParentNode != nullptr) {
      m_ChildNode->ClearCache();
    }
  } else {
    m_ChildNode->RemoveLastInput();
  }
}

NodeDisconnectAction::NodeDisconnectAction(AbstractNode* parent_node, AbstractNode* child_node,
                                           uint32_t child_input_index)
    : m_ParentNode(parent_node), m_ChildNode(child_node), m_ChildInputIndex(child_input_index) {}

void NodeDisconnectAction::Do() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, nullptr);
    m_ChildNode->ClearCache();
  } else {
    m_ChildNode->RemoveLastInput();
  }
}

void NodeDisconnectAction::Undo() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_ParentNode);
  } else {
    m_ChildNode->AppendInput(m_ParentNode);
  }
}

NodeCreateAction::NodeCreateAction(NodeNetwork* network, std::string type_name)
    : m_NodeNetwork(network), m_TypeName(type_name) {
  std::cout << "Creating Node of type: " << m_TypeName << std::endl;
  std::cout << "Num nodes in network : " << m_NodeNetwork->nodes.size() << std::endl;
  m_Node = NodeFactoryRegistry::GetInstance().Create(m_TypeName);
  m_NodeNetwork->AddNode(m_Node);
}

void NodeCreateAction::Do() { m_NodeNetwork->AddNode(m_Node); }

void NodeCreateAction::Undo() { m_NodeNetwork->RemoveNode(m_Node.get()); }

};  // namespace NED