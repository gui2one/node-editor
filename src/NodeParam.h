#ifndef NODEPARAM_H
#define NODEPARAM_H

#pragma once

#include "Event.h"
#include "EventManager.h"
namespace NodeEditor {

class NodeParam{
public:
    NodeParam(const char * _name): name(_name){

    }
    virtual ~NodeParam() = default;	 
    virtual void Display() = 0;
public:
    const char* name;
};

template<typename T>
class Param : public NodeParam
{
public:
    Param(const char * _name, T _value): NodeParam(_name), value(_value){};
    ~Param(){};

    T Eval()
    {
        return value;
    };

    void Display(){
        ImGui::Text("%s -- not implemented", name);
    }

public : 
    T value;

private:

};

template<>
class Param<std::string> : public NodeParam{
public:
    Param(const char * _name, std::string _value): NodeParam(_name), value(_value){};
    ~Param(){};

    std::string Eval(){
        return value;
    }

    void Display(){
      char buffer[2048];
      if(value.length() > 2048) value = value.substr(0, 2048);
      std::copy(value.begin(), value.end(), buffer);
      buffer[value.length()] = 0;
      if (ImGui::InputText(name, buffer, 2048)) {

        value = std::string(buffer);
        ParamChangedEvent event;
        EventManager::GetInstance().Dispatch(event);
      }
    }

public :
    std::string value;
};

template<>
class Param<uint32_t> : public NodeParam{
public:
    Param(const char * _name, uint32_t _value): NodeParam(_name), value(_value){};
    ~Param(){};

    uint32_t Eval(){
        return value;
    }

    void Display(){
        if(ImGui::SliderInt(name, (int *)&value, 0, 100)){
            ParamChangedEvent event;
            EventManager::GetInstance().Dispatch(event);
        }
    }

public:
    uint32_t value;
};
//utils
template<typename T>
T get_param_value(NodeParam* param){
    return static_cast<Param<T>*>(param)->Eval();
}
template<typename T>
void set_param_value(NodeParam* param, T value){
    static_cast<Param<T>*>(param)->value = value;
};

};
#endif