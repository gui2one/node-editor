#ifndef EVENT_H
#define EVENT_H
#pragma once

// #include "ImGuiNode.h"
//  #include "NodeManager.h"

namespace NED {

//// forward declarations
class NodeNetwork;
class AbstractNode;

enum class EventType {
  MouseMove,
  MouseClick,
  MouseDoubleClick,
  MouseRelease,
  KeyPress,

  DropFile,

  NodeConnection,
  NodeDisconnection,
  NodeSelected,
  NodeMoved,

  NodeCreated,
  NodeDeleted,

  ParamChanged,
  ManagerUpdate
};

class Event {
 public:
  virtual ~Event() = default;
  virtual EventType GetType() const = 0;
};

class MouseMoveEvent : public Event {
 public:
  float x, y;

  MouseMoveEvent(float x, float y) : x(x), y(y) {}

  EventType GetType() const override { return EventType::MouseMove; }
};

class MouseClickEvent : public Event {
 public:
  int button;

  MouseClickEvent(int button, float x, float y) : button(button), x(x), y(y) {}

  EventType GetType() const override { return EventType::MouseClick; }

  float x, y;
};

class MouseDoubleClickEvent : public Event {
 public:
  int button;

  MouseDoubleClickEvent(int button, float x, float y) : button(button), x(x), y(y) {}

  EventType GetType() const override { return EventType::MouseDoubleClick; }

  float x, y;
};

class MouseReleaseEvent : public Event {
 public:
  int button;

  MouseReleaseEvent(int button) : button(button) {}

  EventType GetType() const override { return EventType::MouseRelease; }
};

class DropFileEvent : public Event {
 public:
  const char* path;

  DropFileEvent(const char* _path) : path(_path) {}

  EventType GetType() const override { return EventType::DropFile; }
};
class KeyPressEvent : public Event {
 public:
  int key;
  int mods;

  KeyPressEvent(int key, int mods) : key(key), mods(mods) {}

  EventType GetType() const override { return EventType::KeyPress; }
};

class NodeConnectionEvent : public Event {
 public:
  std::shared_ptr<AbstractNode> new_parent_node = nullptr;
  uint32_t new_parent_index = 0;
  std::shared_ptr<AbstractNode> old_parent_node = nullptr;
  uint32_t old_parent_index = 0;
  std::shared_ptr<AbstractNode> child_node = nullptr;
  uint32_t child_index = 0;

  NodeConnectionEvent(std::shared_ptr<AbstractNode> new_parent_node = nullptr, uint32_t new_parent_index = 0,
                      std::shared_ptr<AbstractNode> child_node = nullptr, uint32_t child_index = 0)
      : new_parent_node(new_parent_node),
        new_parent_index(new_parent_index),
        child_node(child_node),
        child_index(child_index) {}
  EventType GetType() const override { return EventType::NodeConnection; }
};

class NodeDisconnectionEvent : public Event {
 public:
  std::shared_ptr<AbstractNode> parent_node = nullptr;
  uint32_t parent_index = 0;
  std::shared_ptr<AbstractNode> child_node = nullptr;
  uint32_t child_index = 0;

  NodeDisconnectionEvent(std::shared_ptr<AbstractNode> parent_node = nullptr, uint32_t parent_index = 0,
                         std::shared_ptr<AbstractNode> child_node = nullptr, uint32_t child_index = 0)
      : parent_node(parent_node), parent_index(parent_index), child_node(child_node), child_index(child_index) {}
  EventType GetType() const override { return EventType::NodeDisconnection; }
};

class NodeCreatedEvent : public Event {
 public:
  NodeNetwork* node_network = nullptr;
  AbstractNode* node = nullptr;

 public:
  NodeCreatedEvent(NodeNetwork* node_network, AbstractNode* node) : node_network(node_network), node(node) {}

  EventType GetType() const override { return EventType::NodeCreated; }
};

class NodeDeletedEvent : public Event {
 public:
  NodeNetwork* node_network = nullptr;
  AbstractNode* node = nullptr;

 public:
  NodeDeletedEvent(NodeNetwork* node_network, AbstractNode* node) : node_network(node_network), node(node) {}

  EventType GetType() const override { return EventType::NodeDeleted; }
};
template <typename T>
class ParamChangedEvent : public Event {
 public:
  AbstractNode* node;
  std::string param_name;
  T old_value;
  T new_value;

 public:
  ParamChangedEvent() : node(nullptr), param_name(""), Event() {}
  ParamChangedEvent(AbstractNode* node, std::string param_name, T new_value, T old_value)
      : node(node), param_name(param_name), new_value(new_value), old_value(old_value), Event() {
    // std::cout << "ParamChanged Event : " << param_name << std::endl;
  }

  EventType GetType() const override { return EventType::ParamChanged; }
};
class ManagerUpdateEvent : public Event {
 public:
  ManagerUpdateEvent() : Event() {}

  EventType GetType() const override { return EventType::ManagerUpdate; }
};

};  // namespace NED

#endif