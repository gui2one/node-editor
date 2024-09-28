#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include <imgui.h>
#include "ImGuiNode.h"
class NodeManager
{
public:
    NodeManager();
    ~NodeManager();

    void Init(ImGuiIO* io);
    void Update();


    inline void SetNodes(std::vector<std::shared_ptr<ImGuiNode>> _nodes) { nodes = _nodes; }
private:
    std::vector<std::shared_ptr<ImGuiNode>> nodes;
    ImGuiIO* mIO;
};

#endif