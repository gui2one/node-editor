#ifndef NODE_EDITOR_BASE_APPLICATION_H
#define NODE_EDITOR_BASE_APPLICATION_H
#pragma once
#include "NodeManager.h"
namespace NED {
class BaseApplication {
 public:
  BaseApplication() {};

  inline NodeManager& GetNodeManager() { return m_NodeManager; }

 public:
  NodeManager m_NodeManager;
};

}  // namespace NED
#endif  // NODE_EDITOR_BASE_APPLICATION_H