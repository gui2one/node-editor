#ifndef NODE_EDITOR_ACTION_MANAGER_H
#define NODE_EDITOR_ACTION_MANAGER_H
#pragma once
#include <memory>
#include <stack>

#include "Action.h"
template <typename T>
class ExposedStack : public std::stack<T> {
 public:
  // Expose the underlying container (c is a protected member of std::stack)
  const auto& getContainer() const { return this->c; }
};
namespace NED {

class ActionManager {
 public:
  ExposedStack<std::shared_ptr<Action>> undoStack;
  ExposedStack<std::shared_ptr<Action>> redoStack;

  std::vector<std::string> GetUndoMessages() { return GetStackMessages(undoStack); }
  std::vector<std::string> GetRedoMessages() { return GetStackMessages(redoStack); }

 private:
  // Private constructor for Singleton
  ActionManager() = default;

  template <typename Stack>
  std::vector<std::string> GetStackMessages(const Stack& stack) const {
    auto container = stack.getContainer();  // Access the underlying container
    std::vector<std::string> messages;
    for (const auto& action : container) {
      messages.push_back(action->message);
    }

    return messages;
  }

 public:
  // Delete copy constructor and assignment operator to enforce singleton properties
  ActionManager(const ActionManager&) = delete;
  ActionManager& operator=(const ActionManager&) = delete;

  // Get the singleton instance
  static ActionManager& GetInstance() {
    static ActionManager instance;  // Thread-safe in C++11 and later
    return instance;
  }
  void executeCommand(std::shared_ptr<Action> command) {
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