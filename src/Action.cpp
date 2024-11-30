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

ParamStringAction::ParamStringAction(AbstractNode* node, std::string param_name, std::string from_value,
                                     std::string to_value)
    : m_ParamName(param_name), m_Node(node), from_value(from_value), to_value(to_value) {
  message = std::format("Changed string param from {} to {}", from_value, to_value);
};
void ParamStringAction::Do() {
  auto param_ptr = Utils::FindParamByName(m_Node, m_ParamName);
  auto cast_ptr = std::dynamic_pointer_cast<Param<std::string>>(param_ptr);
  cast_ptr->Set(to_value);
  // std::cout << "[Action]Changed \"" << m_ParamName << "\" to \"" << to_value << "\"" << std::endl;
}
void ParamStringAction::Undo() {
  auto param_ptr = Utils::FindParamByName(m_Node, m_ParamName);
  auto cast_ptr = std::dynamic_pointer_cast<Param<std::string>>(param_ptr);
  cast_ptr->Set(from_value);
}

};  // namespace NED