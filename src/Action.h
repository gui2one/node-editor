#ifndef NODE_EDITOR_ACTION_H
#define NODE_EDITOR_ACTION_H
#pragma once

#include <format>

#include "ImGuiNode.h"
#include "NodeFactory.h"
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

template <typename T>
class ParamAction : public Action {
 public:
  ParamAction(AbstractNode* node, std::string node_uuid, std::string param_name, T old_value, T new_value)
      : m_ParamName(param_name), m_Node(node), m_NodeUUID(node_uuid), old_value(old_value), new_value(new_value) {}
  virtual ~ParamAction() = default;

  void Do() override {
    auto param = Utils::FindParamByName(m_Node, m_ParamName);
    set_param_value<T>(param, new_value);
  }
  void Undo() override {
    auto param = Utils::FindParamByName(m_Node, m_ParamName);
    set_param_value<T>(param, old_value);
  }

 public:
  std::string m_ParamName;
  AbstractNode* m_Node;
  std::string m_NodeUUID;
  T old_value;
  T new_value;
};

class MoveNodeAction : public Action {
 public:
  MoveNodeAction(AbstractNode* node, ImVec2 from_pos, ImVec2 to_pos);

  void Do() override;
  void Undo() override;

 private:
  ImVec2 from_pos;
  ImVec2 to_pos;
  AbstractNode* m_Node;
};

class NodeConnectAction : public Action {
 public:
  NodeConnectAction(AbstractNode* new_parent_node, AbstractNode* old_parent_node, AbstractNode* child_node,
                    uint32_t child_input_index);

  void Do() override;
  void Undo() override;

 private:
  AbstractNode* m_NewParentNode;
  AbstractNode* m_OldParentNode;
  AbstractNode* m_ChildNode;
  uint32_t m_ChildInputIndex;
};
class NodeDisconnectAction : public Action {
 public:
  NodeDisconnectAction(AbstractNode* parent_node, AbstractNode* child_node, uint32_t child_input_index);

  void Do() override;
  void Undo() override;

 private:
  AbstractNode* m_ParentNode;
  AbstractNode* m_ChildNode;
  uint32_t m_ChildInputIndex;
};

};  // namespace NED

#endif  // NODE_EDITOR_ACTION_H