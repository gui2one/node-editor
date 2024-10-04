#ifndef NODEPARAM_H
#define NODEPARAM_H

#pragma once


class NodeParam{
public:
    NodeParam(const char * _name): name(_name){

    }
    virtual ~NodeParam() = default;	 
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
public : 
    T value;

private:

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
enum NodeParamType{
    Int,
    Float,
    String
};
#endif