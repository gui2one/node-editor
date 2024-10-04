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
    Param(const char * name):NodeParam(name){};
    ~Param(){};

    T Eval()
    {
        return value;
    };
public : 
    T value;

private:

};

#endif