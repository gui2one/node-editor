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

void NodeConnectAction::Do() { m_ChildNode->SetInput(m_ChildInputIndex, m_ParentNode); }

void NodeConnectAction::Undo() {
  m_ChildNode->SetInput(m_ChildInputIndex, nullptr);
  m_ChildNode->ClearCache();
}

};  // namespace NED