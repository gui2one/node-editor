#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#pragma once
#include "ImguiNode.h"
#include <type_traits>
#include <iostream>

class MeshOperator{
public:
    MeshOperator(){};
    virtual ~MeshOperator() = default;
    virtual void Generate() = 0;
};

class MeshGenerator : public MeshOperator
{
public:
    MeshGenerator();
    ~MeshGenerator();

    virtual void Generate() = 0;
private:

};

class GridGenerator : public MeshGenerator
{
public:
    GridGenerator();
    ~GridGenerator();

    void Generate() override{
        std::cout << "Grid Generator !!!!!" << std::endl;
        
    }

private:
};


template <typename T>
class Node : public ImGuiNode, public T
{
    static_assert(std::is_base_of<MeshOperator, T>::value, "T must be derived from MeshGenerator");
public:
    Node(const char * _title) : ImGuiNode(_title) {}

    
    void Update() {
        auto ptr = static_cast<T*>(this);
        ptr->Generate();

    }
};
#endif