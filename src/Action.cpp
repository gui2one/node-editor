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

};  // namespace NED