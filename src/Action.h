#ifndef NODE_EDITOR_ACTION_H
#define NODE_EDITOR_ACTION_H
#pragma once

#include "NodeParam.h"
#include "utils/node_manager_utils.h"
namespace NED {

class Action {
 public:
  virtual ~Action() = default;
  virtual void Do() {}
  virtual void Undo() {}
};

class MoveNodeAction : public Action {
 private:
  ImVec2 from_pos;
  ImVec2 to_pos;
  std::shared_ptr<AbstractNode> m_Node;

 public:
  MoveNodeAction(std::shared_ptr<AbstractNode> node, ImVec2 from_pos, ImVec2 to_pos)
      : from_pos(from_pos), to_pos(to_pos), m_Node(node) {

        };

  void Do() override {
    m_Node->position = to_pos;
    std::cout << "[Action]Moved \"" << m_Node->title << "\" to " << m_Node->position.x << ", " << m_Node->position.y
              << std::endl;
  }
  void Undo() override { m_Node->position = from_pos; }
};

// Param Actions
class ParamStringAction : public Action {
 private:
  std::string m_ParamName;
  std::shared_ptr<AbstractNode> m_Node;
  std::string from_value;
  std::string to_value;

 public:
  ParamStringAction(std::shared_ptr<AbstractNode> node, std::string param_name, std::string from_value,
                    std::string to_value)
      : m_ParamName(param_name), m_Node(node), from_value(from_value), to_value(to_value) {};

  void Do() override {
    auto param_ptr = Utils::FindParamByName(m_Node, m_ParamName);
    std::cout << "[Action]Changed \"" << m_ParamName << "\" to \"" << to_value << "\"" << std::endl;
  }

  void Undo() override {
    std::cout << "[Action]Changed \"" << m_ParamName << "\" to \"" << from_value << "\"" << std::endl;
  }
};
};  // namespace NED

#endif  // NODE_EDITOR_ACTION_H