#ifndef NODEPARAM_H
#define NODEPARAM_H


#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Event.h"
#include "EventManager.h"
#include "utils.h"
// #include "yaml_serialize.h"

#define NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC() \
    YAML::Node YAMLSerialize() override {\
        YAML::Node yaml_node;\
        std::string type_str = typeid(*this).name();\
        NodeEditor::str_remove_all(type_str,"class ");\
        NodeEditor::str_remove(type_str,"NodeEditor::Param<");\
        NodeEditor::str_remove_last(type_str,">");\
        yaml_node["type"] = type_str;\
        yaml_node["name"] = name;\
        yaml_node["value"] = value;\
        return yaml_node;\
    }

namespace NodeEditor {

class NodeParam{
public:
    NodeParam(const char * _name): name(_name){

        
    }   
    virtual ~NodeParam() = default;	 
    virtual void Display() = 0;
    virtual YAML::Node YAMLSerialize() = 0;
public:
    const char* name;
};

template<typename T>
class Param : public NodeParam{
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

    YAML::Node YAMLSerialize() override {
        YAML::Node yaml_node;
        yaml_node["type"] = typeid(*this).name();
        yaml_node["name"] = name;
        yaml_node["value"] = value;
        return yaml_node;
    }

public : 
    T value;

private:

};


template<>
class Param<glm::vec2> : public NodeParam{
public:
    Param(const char * _name, glm::vec2 _value): NodeParam(_name), value(_value){};
    ~Param(){};

    glm::vec2 Eval(){
        return value;
    }

    void Display(){
        if(ImGui::DragFloat2(name, glm::value_ptr(value))){
            ParamChangedEvent event;
            EventManager::GetInstance().Dispatch(event);
        }
    }
    NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
public:
    glm::vec2 value;
};

template<>
class Param<glm::vec3> : public NodeParam{
public:
    Param(const char * _name, glm::vec3 _value): NodeParam(_name), value(_value){};
    ~Param(){};

    glm::vec3 Eval(){
        return value;
    }

    void Display(){
        if(ImGui::DragFloat3(name, glm::value_ptr(value))){
            ParamChangedEvent event;
            EventManager::GetInstance().Dispatch(event);
        }
    }
    NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
public:
    glm::vec3 value;
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

    NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

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
    NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();    

public:
    uint32_t value;
};
template<>

class Param<float> : public NodeParam{
public:
    Param(const char * _name, float _value): NodeParam(_name), value(_value){};
    ~Param(){};

    float Eval(){
        return value;
    }

    void Display(){
        if(ImGui::SliderFloat(name, &value, 0, 100)){
            ParamChangedEvent event;
            EventManager::GetInstance().Dispatch(event);
        }
    }
    NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();    

public:
    float value;
};

template<>
class Param<bool> : public NodeParam{
public:
    Param(const char * _name, bool _value): NodeParam(_name), value(_value){};
    ~Param(){};

    bool Eval(){
        return value;
    }

    void Display(){
        if(ImGui::Checkbox(name, &value)){
            ParamChangedEvent event;
            EventManager::GetInstance().Dispatch(event);
        }
    }
    NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
public:
    bool value;
};

//utils
template<typename T>
T get_param_value(std::shared_ptr<NodeParam> param){
    return static_cast<Param<T>*>(param.get())->Eval();
}
template<typename T>
void set_param_value(std::shared_ptr<NodeParam> param, T value){
    static_cast<Param<T>*>(param.get())->value = value;
};

};
#endif