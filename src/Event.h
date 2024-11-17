#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "ImGuiNode.h"

namespace NED {

// forward declarations
class AbstractNode;

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
  std::shared_ptr<AbstractNode> input_node;
  uint32_t input_index;
  std::shared_ptr<AbstractNode> output_node;
  uint32_t output_index;

  NodeConnectionEvent(std::shared_ptr<AbstractNode> input_node, uint32_t input_index,
                      std::shared_ptr<AbstractNode> output_node, uint32_t output_index) {}
  EventType GetType() const override { return EventType::NodeConnection; }
};

class ParamChangedEvent : public Event {
 public:
  ParamChangedEvent() : Event() {}

  EventType GetType() const override { return EventType::ParamChanged; }
};
class ManagerUpdateEvent : public Event {
 public:
  ManagerUpdateEvent() : Event() {}

  EventType GetType() const override { return EventType::ManagerUpdate; }
};

};  // namespace ned

#endif