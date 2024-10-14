#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "EventDispatcher.h"
namespace NodeEditor {
class EventManager {
public:
  static EventDispatcher &GetInstance() {
    static EventDispatcher dispatcher;
    return dispatcher;
  }
};
}; // namespace NodeEditor

#endif
