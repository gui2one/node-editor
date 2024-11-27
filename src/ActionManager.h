#ifndef NODE_EDITOR_ACTION_MANAGER_H
#define NODE_EDITOR_ACTION_MANAGER_H
#pragma once
#include <memory>
#include <stack>

#include "Action.h"
namespace NED {
class ActionManager {
 private:
  std::stack<std::unique_ptr<Action>> undoStack;
  std::stack<std::unique_ptr<Action>> redoStack;

 public:
  void executeCommand(std::unique_ptr<Action> command) {
    command->Do();
    undoStack.push(std::move(command));
    while (!redoStack.empty()) redoStack.pop();
  }

  void undo() {
    if (!undoStack.empty()) {
      auto command = std::move(undoStack.top());
      undoStack.pop();
      command->Undo();
      redoStack.push(std::move(command));
    }
  }

  void redo() {
    if (!redoStack.empty()) {
      auto command = std::move(redoStack.top());
      redoStack.pop();
      command->Do();
      undoStack.push(std::move(command));
    }
  }
};
};  // namespace NED
#endif  // NODE_EDITOR_ACTION_MANAGER_H