#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#pragma once

#include <vector>
#include <functional>
#include "Event.h"

class EventDispatcher {
public:
    using EventCallback = std::function<void(const Event&)>;

    void Subscribe(EventType type, const EventCallback& callback) {
        listeners[type].push_back(callback);
    }

    void Dispatch(const Event& event) {
        auto& callbacks = listeners[event.GetType()];
        for (const auto& callback : callbacks) {
            callback(event);
        }
    }

private:
    std::unordered_map<EventType, std::vector<EventCallback>> listeners;
};

#endif