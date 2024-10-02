#ifndef EVENT_H
#define EVENT_H
#pragma once

enum class EventType {
    MouseMove,
    MouseClick,
    KeyPress,
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

// Define more events as needed, e.g., for keypress, node actions, etc.


#endif