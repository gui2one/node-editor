#ifndef NODE_EDITOR_ACTION_H
#define NODE_EDITOR_ACTION_H
#pragma once

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
};  // namespace NED

#endif  // NODE_EDITOR_ACTION_H