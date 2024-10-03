#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#pragma once
#include "ImguiNode.h"
#include <type_traits>
#include <iostream>
#include <string>

class StringOperator : public ImGuiNode{
public:
    StringOperator(): ImGuiNode("default"){};
    virtual ~StringOperator() = default;
    virtual void Generate() = 0;

public :
    std::string m_StringCache = "";

    std::array<StringOperator*, 4> inputs = { nullptr, nullptr, nullptr, nullptr }; 
};

class StringGenerator : public StringOperator
{
public:
    StringGenerator();
    ~StringGenerator();
    virtual void Generate() = 0;
private:

};

class HelloGenerator : public StringGenerator
{
public:
    HelloGenerator();
    ~HelloGenerator();

    void Generate() override{
        m_StringCache = "Hello";
    }
private:
};

class WorldGenerator : public StringGenerator
{
public:
    WorldGenerator();
    ~WorldGenerator();

    void Generate() override{
        m_StringCache = "World";
    }
private:
};

class StringModifier : public StringOperator
{
public:
    StringModifier();
    ~StringModifier();
    virtual void Generate() = 0;
private:

};

class StringConcatenator : public StringModifier
{
public:
    StringConcatenator();
    ~StringConcatenator();

    void Generate() override{
        if( inputs[0] != nullptr && inputs[1] != nullptr) {
            m_StringCache = inputs[0]->m_StringCache + inputs[1]->m_StringCache;        
        }
    }
};

template <typename T>
class Node : public T
{
    static_assert(std::is_base_of<StringOperator, T>::value, "T must be derived from StringOperator");
public:
    Node(const char * _title) {
        auto node = static_cast<ImGuiNode*>(this);
        if(std::is_base_of<StringGenerator, T>::value) {
            node->color = NODE_COLOR::MAROON;
            node->title = _title;
        }else if(std::is_base_of<StringModifier, T>::value) {
            node->color = NODE_COLOR::DARK_GREEN;
            node->title = _title;
        }
    }

    void Update() {
        auto node = static_cast<ImGuiNode*>(this);
        auto op = static_cast<StringOperator*>(this);
        for(uint32_t i = 0; i < MAX_N_INPUTS; i++) {
            if(node->GetInput(i) != nullptr) {
                node->GetInput(i)->Update(); /* Important !!*/
                op->inputs[i] = static_cast<StringOperator*>(node->GetInput(i).get());
                op->inputs[i]->Generate();
                // std::cout << node->title << " -> " << op->inputs[i]->m_StringCache << std::endl;
            }
        }
        op->Generate();

        // 'final' output
        std::cout << op->m_StringCache << std::endl;
    }
};

#endif