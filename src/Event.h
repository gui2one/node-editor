#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "ImGuiNode.h"

namespace NED {

//// forward declarations
// class AbstractNode;

enum class EventType {
  MouseMove,
  MouseClick,
  MouseDoubleClick,
  MouseRelease,
  KeyPress,

  DropFile,

  NodeConnection,
  NodeSelected,
  NodeMoved,

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
  std::shared_ptr<AbstractNode> input_node = nullptr;
  uint32_t input_index = 0;
  std::shared_ptr<AbstractNode> output_node = nullptr;
  uint32_t output_index = 0;

  NodeConnectionEvent(std::shared_ptr<AbstractNode> input_node = nullptr, uint32_t input_index = 0,
                      std::shared_ptr<AbstractNode> output_node = nullptr, uint32_t output_index = 0)
      : input_node(input_node), input_index(input_index), output_node(output_node), output_index(output_index) {}
  EventType GetType() const override { return EventType::NodeConnection; }
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