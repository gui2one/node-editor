#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#pragma once

#include "EventDispatcher.h"
namespace NED {
class EventManager {
 public:
  static EventDispatcher &GetInstance() {
    static EventDispatcher dispatcher;
    return dispatcher;
  }
};
};  // namespace ned

#endif
