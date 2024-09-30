#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#pragma once
#include "ImguiNode.h"
#include <type_traits>
#include <iostream>

class StringOperator{
public:
    StringOperator(){};
    virtual ~StringOperator() = default;
    virtual void Generate() = 0;
};

class StringGenerator : public StringOperator
{
public:
    StringGenerator();
    ~StringGenerator();

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
class Node : public ImGuiNode, public T
{
    static_assert(std::is_base_of<StringOperator, T>::value, "T must be derived from SpringOperator");
public:
    Node(const char * _title) : ImGuiNode(_title) {
        if(std::is_base_of<StringGenerator, T>::value) {
            
            color = NODE_COLOR::MAROON;
        }else if(std::is_base_of<StringModifier, T>::value) {
            
            color = NODE_COLOR::DARK_GREEN;
        }
    }

    
    void Update() {
        auto ptr = static_cast<T*>(this);
        ptr->Generate();

    }
};
#endif