#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#pragma once
#include "ImguiNode.h"
class MeshGenerator : public ImGuiNode
{
public:
    MeshGenerator(const char * _title);
    ~MeshGenerator();

private:

};

class GridGenerator : public MeshGenerator
{
public:
    GridGenerator(const char * _title);
    ~GridGenerator();

private:
};

#endif