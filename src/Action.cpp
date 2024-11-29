#include "Action.h"

namespace NED {
MoveNodeAction::MoveNodeAction(std::shared_ptr<AbstractNode> node, ImVec2 from_pos, ImVec2 to_pos)
    : from_pos(from_pos), to_pos(to_pos), m_Node(node) {}

void MoveNodeAction::Do() {
  m_Node->position = to_pos;
  std::cout << "[Action]Moved \"" << m_Node->title << "\" to " << m_Node->position.x << ", " << m_Node->position.y
            << std::endl;
}
void MoveNodeAction::Undo() { m_Node->position = from_pos; }

ParamStringAction::ParamStringAction(std::shared_ptr<AbstractNode> node, std::string param_name, std::string from_value,
                                     std::string to_value)
    : m_ParamName(param_name), m_Node(node), from_value(from_value), to_value(to_value) {};
void ParamStringAction::Do() {
  auto param_ptr = Utils::FindParamByName(m_Node, m_ParamName);
  std::cout << "[Action]Changed \"" << m_ParamName << "\" to \"" << to_value << "\"" << std::endl;
}
void ParamStringAction::Undo() {
  std::cout << "[Action]Changed \"" << m_ParamName << "\" to \"" << from_value << "\"" << std::endl;
}

};  // namespace NED