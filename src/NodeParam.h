#ifndef NODEPARAM_H
#define NODEPARAM_H
#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include "Event.h"
#include "EventManager.h"
#include "utils.h"

#define NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC() \
    YAML::Node YAMLSerialize() override {\
        YAML::Node yaml_node;\
        std::string type_str = typeid(*this).name();\
        NodeEditor::str_remove_all(type_str,"class ");\
        NodeEditor::str_remove_all(type_str,"struct ");\
        NodeEditor::str_remove(type_str,"NodeEditor::Param<");\
        NodeEditor::str_remove_last(type_str,">");\
        NodeEditor::str_remove_last(type_str," ");\
        if(type_str.find("std::basic_string") != std::string::npos) type_str = "std::string";\
        yaml_node["type"] = type_str;\
        yaml_node["name"] = name;\
        yaml_node["value"] = value;\
        return yaml_node;\
    }

#define DISPATCH_PARAM_CHANGE_EVENT() \
    ParamChangedEvent event;\
    EventManager::GetInstance().Dispatch(event);
#define DISPATCH_EDITOR_UPDATE_EVENT() \
    ManagerUpdateEvent event;\
    EventManager::GetInstance().Dispatch(event);

#define DISPLAY_PARAM_TEMPLATE(label, func) \
    ImGui::PushID(name);\
    ImGui::Columns(2);\
    ImGui::SetColumnWidth(0, 90.f);\
    ImGui::Text("%s", name);\
    ImGui::NextColumn();\
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);\
    func();\
    ImGui::PopItemWidth();\
    ImGui::Columns(1);\
    ImGui::PopID();


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

class ParamGroup : public NodeParam{
public:
    ParamGroup(const char * _name): NodeParam(_name){
        std::cout << "Constructing ParamGroup -----------" << std::endl;
        
    };
    ~ParamGroup(){};
    void Display() override;

    YAML::Node YAMLSerialize() override {
        YAML::Node yaml_node;
        std::string type_str = typeid(*this).name();
        str_remove_all(type_str,"class ");\
        str_remove(type_str,"NodeEditor::");\
        yaml_node["type"] = type_str;
        yaml_node["name"] = name;
        
        for(auto item : items) {
            yaml_node["params"].push_back(item->YAMLSerialize());            
        }
        yaml_node["value"] = "null";
        return yaml_node;
    }
public:
    std::vector<std::shared_ptr<NodeParam>> items;
};

class ParamSeparator : public NodeParam{
public:
    ParamSeparator(const char * _name): NodeParam(_name){};
    ~ParamSeparator(){};
    void Display(){
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
    YAML::Node YAMLSerialize() override {
        YAML::Node yaml_node;
        std::string type_str = typeid(*this).name();
        str_remove_all(type_str,"class ");\
        str_remove(type_str,"NodeEditor::");\
        yaml_node["type"] = type_str;
        yaml_node["name"] = name;
        yaml_node["value"] = "null";
        return yaml_node;
    }
};

class ParamLabel : public NodeParam{
public:
    ParamLabel(const char * _name): NodeParam(_name){};
    ~ParamLabel(){};
    void Display(){
        ImGui::Spacing();
        ImGui::Text("%s", name);
    }
    YAML::Node YAMLSerialize() override {
        YAML::Node yaml_node;
        std::string type_str = typeid(*this).name();
        str_remove_all(type_str,"class ");\
        str_remove(type_str,"NodeEditor::");\
        yaml_node["type"] = type_str;
        yaml_node["name"] = name;
        yaml_node["value"] = "null";
        return yaml_node;
    }
};

class ParamComboBox : public NodeParam{
public:
    ParamComboBox(const char * _name): NodeParam(_name){};
    ~ParamComboBox(){};
    void Display(){
        ImGui::Spacing();
        if(ImGui::Combo(name, &value, choices.data(), static_cast<int>(choices.size()))){
            // DISPATCH_PARAM_CHANGE_EVENT();
            DISPATCH_EDITOR_UPDATE_EVENT();
        }
    }

    inline int GetChoice() { return value; }
    inline void SetChoice(int choice) { value = choice; }
    inline void SetChoices(std::vector<const char*> _choices) { choices = _choices; }

    YAML::Node YAMLSerialize() override {
        YAML::Node yaml_node;
        std::string type_str = typeid(*this).name();
        str_remove_all(type_str,"class ");\
        str_remove(type_str,"NodeEditor::");\
        yaml_node["type"] = type_str;
        yaml_node["name"] = name;
        yaml_node["value"] = value;
        return yaml_node;
    }

public : 
    int value = 0;
    std::vector<const char*> choices = {"first", "second", "third"};
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
        DISPLAY_PARAM_TEMPLATE(name, [this](){
            float default_value = 0.0f;


            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.8f, 0.1f, 0.1f, 1.f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.9f, 0.1f, 0.1f, 1.f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(1.0f, 0.1f, 0.1f, 1.f)));

            float avail_x = ImGui::GetContentRegionAvail().x;
            float spacing = ImGui::GetCurrentContext()->Style.ItemInnerSpacing.x;
            float input_width = avail_x / 3.0f - 30.0f - spacing;
            ImGui::PushID(0);
            if (ImGui::Button("X", ImVec2(30, 25)))
            {
                value.x = default_value;
                DISPATCH_PARAM_CHANGE_EVENT();
            }
            ImGui::PopID();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            ImGui::PushID(1);
            ImGui::PushItemWidth(input_width);
            if(ImGui::DragFloat("##x", &value.x, 0.05f)){
                DISPATCH_PARAM_CHANGE_EVENT();
            }
            ImGui::PopID();
            ImGui::PopItemWidth();

            ImGui::SameLine(0, spacing);
            

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.5f, 0.1f, 1.f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.6f, 0.1f, 1.f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.7f, 0.1f, 1.f)));
            ImGui::PushID("label_y");
            
            if (ImGui::Button("Y", ImVec2(30, 25)))
            {
                value.y = default_value;
            }
            ImGui::PopID();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::PushID("float_y");
            ImGui::PushItemWidth(input_width);
            if(ImGui::DragFloat("##y", &value.y, 0.05f)){
                DISPATCH_PARAM_CHANGE_EVENT();
            }
            ImGui::PopID();
            ImGui::PopItemWidth();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.1f, 0.8f, 1.f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.1f, 0.9f, 1.f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.1f, 1.0f, 1.f)));
            ImGui::SameLine(0, spacing);
            ImGui::PushID("label_z");
            if (ImGui::Button("Z", ImVec2(30, 25)))
            {
                value.z = default_value;
            }
            ImGui::PopID();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::PushID("float_z");
            ImGui::PushItemWidth(input_width);
            if(ImGui::DragFloat("##z", &value.z, 0.05f)){
                DISPATCH_PARAM_CHANGE_EVENT();
            }
            ImGui::PopID();
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();

        });
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
        DISPLAY_PARAM_TEMPLATE(name, [this]() {
            char buffer[2048];
            if(value.length() > 2048) value = value.substr(0, 2048);
            std::copy(value.begin(), value.end(), buffer);
            buffer[value.length()] = 0;
            if (ImGui::InputText("##name", buffer, 2048)) {

                value = std::string(buffer);
                DISPATCH_PARAM_CHANGE_EVENT();
            }
        });

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
            DISPATCH_PARAM_CHANGE_EVENT();
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
            DISPATCH_PARAM_CHANGE_EVENT();
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
            DISPATCH_PARAM_CHANGE_EVENT();
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