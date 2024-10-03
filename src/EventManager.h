#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#pragma once

#include "EventDispatcher.cpp"
class EventManager {
public:
    static EventDispatcher& GetInstance() {
        static EventDispatcher dispatcher;
        return dispatcher;
    }
};

#endif