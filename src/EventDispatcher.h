#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
#pragma once

#include <functional>
#include <vector>

#include "Event.h"

namespace NED {

class EventDispatcher {
 public:
  using EventCallback = std::function<void(const Event &)>;

  void Subscribe(EventType type, const EventCallback &callback) { listeners[type].push_back(callback); }

  void Dispatch(const Event &event) {
    auto &callbacks = listeners[event.GetType()];
    for (const auto &callback : callbacks) {
      callback(event);
    }
  }

 private:
  std::unordered_map<EventType, std::vector<EventCallback>> listeners;
};
};  // namespace ned

#endif