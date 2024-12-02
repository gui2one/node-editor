#include "Action.h"

namespace NED {
MoveNodeAction::MoveNodeAction(AbstractNode* node, ImVec2 from_pos, ImVec2 to_pos)
    : from_pos(from_pos), to_pos(to_pos), m_Node(node) {
  message = std::format("Moved Node from {},{} to {},{}", from_pos.x, from_pos.y, to_pos.x, to_pos.y);
}

void MoveNodeAction::Do() {
  m_Node->position = to_pos;
  std::cout << message << std::endl;
}
void MoveNodeAction::Undo() { m_Node->position = from_pos; }

NodeConnectAction::NodeConnectAction(AbstractNode* parent_node, AbstractNode* child_node, uint32_t child_input_index)
    : m_ParentNode(parent_node), m_ChildNode(child_node), m_ChildInputIndex(child_input_index) {}

void NodeConnectAction::Do() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_ParentNode);
  } else {
    m_ChildNode->AppendInput(m_ParentNode);
  }
}

void NodeConnectAction::Undo() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, nullptr);
    m_ChildNode->ClearCache();
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

};  // namespace NED