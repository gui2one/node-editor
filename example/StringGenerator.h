#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#pragma once
#include "ImguiNode.h"
#include <type_traits>
#include <iostream>

class StringOperator : public ImGuiNode{
public:
    StringOperator(): ImGuiNode("default"){};
    virtual ~StringOperator() = default;
    virtual void Generate() = 0;

public :

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
        std::cout << "Hello Generator !!!!!" << std::endl;
        
    }
private:
};

class StringModifier : public StringOperator
{
public:
    StringModifier();
    ~StringModifier();

private:

};

class StringConcatenator : public StringModifier
{
public:
    StringConcatenator();
    ~StringConcatenator();

    void Generate() override{
        std::cout << "String Concatenator !!!!!" << std::endl;
        
    }
};

template <typename T>
class Node : public T
{
    static_assert(std::is_base_of<StringOperator, T>::value, "T must be derived from SpringOperator");
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
        auto ptr = static_cast<T*>(this);
        ptr->Generate();

    }
};
#endif