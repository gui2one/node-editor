#ifndef NODE_EDITOR_ACTION_H
#define NODE_EDITOR_ACTION_H
#pragma once

#include <format>

#include "ImGuiNode.h"
#include "NodeParam.h"
#include "utils/node_manager_utils.h"
namespace NED {

class Action {
 public:
  virtual ~Action() = default;
  virtual void Do() {}
  virtual void Undo() {}

 public:
  std::string message = "no message";
};

class MoveNodeAction : public Action {
 public:
  MoveNodeAction(std::shared_ptr<AbstractNode> node, ImVec2 from_pos, ImVec2 to_pos);

  void Do() override;
  void Undo() override;

 private:
  ImVec2 from_pos;
  ImVec2 to_pos;
  std::shared_ptr<AbstractNode> m_Node;
};

// Param Actions
class ParamStringAction : public Action {
 public:
  ParamStringAction(std::shared_ptr<AbstractNode> node, std::string param_name, std::string from_value,
                    std::string to_value);

  void Do() override;
  void Undo() override;

 private:
  std::string m_ParamName;
  std::shared_ptr<AbstractNode> m_Node;
  std::string from_value;
  std::string to_value;
};
};  // namespace NED

#endif  // NODE_EDITOR_ACTION_H