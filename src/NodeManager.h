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

    void Update();


    inline void SetNodes(std::vector<std::shared_ptr<ImGuiNode>> _nodes) { nodes = _nodes; }
    inline void AddNode(std::shared_ptr<ImGuiNode> _node) { nodes.push_back(_node); }

    inline std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() { return nodes; }
    inline const std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() const { return nodes; }

private:
    std::vector<std::shared_ptr<ImGuiNode>> nodes;
};

#endif