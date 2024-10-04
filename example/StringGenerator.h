#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#pragma once
#include "ImguiNode.h"
#include "NodeParam.h"
#include <type_traits>
#include <iostream>
#include <string>
namespace NodeEditor {

class StringOperator : public ImGuiNode{
public:
    StringOperator(): ImGuiNode("default"){};
    virtual ~StringOperator() = default;
    virtual void Generate() = 0;

public :
    std::string m_StringCache = "";

    // std::array<StringOperator*, 4> inputs = { nullptr, nullptr, nullptr, nullptr }; 
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
class StringGenerate : public StringGenerator
{
public:
    StringGenerate(){
        value = std::make_shared<Param<std::string>>("value", "Hello");
        m_Params.push_back(value);
    };
    ~StringGenerate(){};

    void Generate() override{
        m_StringCache = get_param_value<std::string>(value.get());
    }
public:

    std::shared_ptr<Param<std::string>> value;
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
        if( GetInput(0) != nullptr && GetInput(1) != nullptr) {
            auto op0 = static_cast<StringOperator*>(GetInput(0).get());
            auto op1 = static_cast<StringOperator*>(GetInput(1).get());
            m_StringCache = op0->m_StringCache + op1->m_StringCache;        
        }
    }
};

class StringRepeater : public StringModifier{
public:
    StringRepeater();
    ~StringRepeater();

    void Generate() override{
        if( GetInput(0) != nullptr) {
            std::string val = "";
            auto op0 = static_cast<StringOperator*>(GetInput(0).get());
            auto param_0 = static_cast<Param<uint32_t>*>(m_Params[0].get());
            auto val_0 = get_param_value<uint32_t>(param_0);
            for(uint32_t i = 0; i < param_0->Eval(); i++) {
                val += op0->m_StringCache;
            }
            m_StringCache = val;
        }
    }

public:
    uint32_t m_Count = 2;
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
                auto opinput = static_cast<StringOperator*>(node->GetInput(i).get());
                op->SetInput(i, node->GetInput(i));
                opinput->Generate();
                // std::cout << node->title << " -> " << op->inputs[i]->m_StringCache << std::endl;
            }
        }
        op->Generate();

        // 'final' output
        // std::cout << op->m_StringCache << std::endl;
    }
};
};

#endif