#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "ImGuiNode.h"
namespace NodeEditor {

enum class EventType {
    MouseMove,
    MouseClick,
    MouseRelease,
    KeyPress,
    NodeConnection,
    NodeSelected,
    NodeMoved,
    // Add more events as needed
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

    EventType GetType() const override {
        return EventType::MouseMove;
    }
};

class MouseClickEvent : public Event {
public:
    int button;

    MouseClickEvent(int button) : button(button) {}

    EventType GetType() const override {
        return EventType::MouseClick;
    }
};

class MouseReleaseEvent : public Event {
public:
    int button;

    MouseReleaseEvent(int button) : button(button) {}

    EventType GetType() const override {
        return EventType::MouseRelease;
    }
};

class KeyPressEvent : public Event {
public:
    int key;

    KeyPressEvent(int key) : key(key) {}

    EventType GetType() const override {
        return EventType::KeyPress;
    }
};

class NodeConnectionEvent : public Event {
public:
    std::shared_ptr<ImGuiNode> input_node;
    uint32_t input_index;
    std::shared_ptr<ImGuiNode> output_node;
    uint32_t output_index;

    NodeConnectionEvent(std::shared_ptr<ImGuiNode> input_node, uint32_t input_index, std::shared_ptr<ImGuiNode> output_node, uint32_t output_index) {}
    EventType GetType() const override {
        return EventType::NodeConnection;
    }    
};

// Define more events as needed, e.g., for keypress, node actions, etc.

};

#endif