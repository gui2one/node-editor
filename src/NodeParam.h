#ifndef NODEPARAM_H
#define NODEPARAM_H
#pragma once

#include <imgui_internal.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ActionManager.h"
#include "Event.h"
#include "EventManager.h"
#include "pch.h"
#include "ui_utils.h"
#include "utils.h"
#include "utils/node_manager_utils.h"

#define NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC()     \
  YAML::Node YAMLSerialize() override {             \
    YAML::Node yaml_node;                           \
    std::string type_str = std::string(m_TypeName); \
    yaml_node["type"] = type_str;                   \
    yaml_node["label"] = m_Label;                   \
    yaml_node["value"] = value;                     \
    return yaml_node;                               \
  }

#define DISPATCH_PARAM_CHANGE_EVENT(Type, Node, ParamName, NewValue, OldValue) \
  ParamChangedEvent<Type> event(Node, ParamName, NewValue, OldValue);          \
  EventManager::GetInstance().Dispatch(event);

#define DISPATCH_EDITOR_UPDATE_EVENT() \
  ManagerUpdateEvent _event;           \
  EventManager::GetInstance().Dispatch(_event);

//#define DISPLAY_PARAM_TEMPLATE(label, func)               \
//  ImGui::PushID(m_Label);                                 \
//  ImGui::Columns(2);                                      \
//  ImGui::SetColumnWidth(0, 150.f);                        \
//  ImGui::Text("%s", m_Label);                             \
//  ImGui::NextColumn();                                    \
//  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); \
//  func();                                                 \
//  ImGui::PopItemWidth();                                  \
//  ImGui::Columns(1);                                      \
//  ImGui::PopID();                                         \
//  ImGui::Spacing();

namespace NED {
// forward declare AbstratcNode
class AbstractNode;

class NodeParam {
 public:
  NodeParam() : m_Label("no label"), m_TypeName("no type") {}
  NodeParam(const char* _name) : m_Label(_name), m_TypeName("no type") {}
  virtual ~NodeParam() = default;
  virtual void Display() = 0;
  virtual void DisplayOptions() = 0;

  virtual YAML::Node YAMLSerialize() = 0;
  virtual void YAMLDeserialize(YAML::Node yaml_node) = 0;

  virtual void Clone(std::shared_ptr<NodeParam> dst_param) = 0;

 public:
  const char* m_Label;
  const char* m_TypeName;
  AbstractNode* m_Node = nullptr;
};

template <typename T>
class Param : public NodeParam {
 public:
  Param() = default;
  Param(const char* _name, T _value, T _min_val, T _max_val, T _default_val)
      : NodeParam(_name),
        value(_value),
        min_val(_min_val),
        max_val(_max_val),
        default_val(_default_val),
        old_value(_value) {};
  ~Param() {};

  T Eval() { return value; };
  void Clone(std::shared_ptr<NodeParam> dst_param) override {
    auto cast_p = std::dynamic_pointer_cast<Param<T>>(dst_param);

    if (cast_p != nullptr) {
      cast_p->Set(value, min_val, max_val);
    } else {
      std::cout << "---------- Param cast failed ---------------" << std::endl;
    }
  }
  void Set(T _value) {
    temp_value = _value;
    value = _value;
  };

  void Set(T _value, T _min_val, T _max_val) {
    temp_value = _value;
    value = _value;

    min_val = _min_val;
    max_val = _max_val;
  };

  void Display() { ImGui::Text("%s -- not implemented", m_Label); }
  void DisplayOptions() { ImGui::Text("%s -- Options not implemented", m_Label); }

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    yaml_node["type"] = std::string(m_TypeName);
    yaml_node["label"] = m_Label;
    yaml_node["value"] = value;
    yaml_node["min_val"] = min_val;
    yaml_node["max_val"] = max_val;
    return yaml_node;
  }

  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<T>()); }

 public:
  T value;
  T old_value;
  T min_val;
  T max_val;
  T default_val;
  T temp_value;
};

/* derived classes */
class ParamFloat : public Param<float> {
 public:
  ParamFloat() {
    min_val = std::numeric_limits<float>::min();
    max_val = std::numeric_limits<float>::max();
  }
  void Display();
  void DisplayOptions();
};

class ParamInt : public Param<int> {
 public:
  ParamInt() {
    min_val = std::numeric_limits<int>::min();
    max_val = std::numeric_limits<int>::max();
  }
  void Display();
  void DisplayOptions();
};

class ParamBool : public Param<bool> {
 public:
  void Display();
};

class ParamString : public Param<std::string> {
 public:
  void Display();
};

class ParamVec2 : public Param<glm::vec2> {
 public:
  void Display();
};

class ParamVec3 : public Param<glm::vec3> {
 public:
  void Display();
};

class ParamColor : public Param<glm::vec3> {
 public:
  void Display();
};

class ParamFile : public Param<std::wstring> {
 public:
  std::vector<Utils::FileFilterItem> filters = {{"All Files", "*"}, {"Text Files", "txt"}};

 public:
  void Display();
  // unable to use the MACRO here because of wide_to_utf8() call
  YAML::Node YAMLSerialize() override;

  inline void YAMLDeserialize(YAML::Node yaml_node) { Set(yaml_node["value"].as<std::wstring>()); }
};

class ParamLabel : public Param<std::string> {
 public:
  void Display();
};

class ParamComboBox : public Param<int> {
 public:
  void Display();

  inline int GetChoice() { return value; }
  inline void SetChoice(int choice) { value = choice; }
  inline void SetChoices(std::vector<const char*> _choices) { choices = _choices; }

 public:
  std::vector<const char*> choices = {"first", "second", "third"};
};

class ParamSeparator : public Param<std::string> {
 public:
  void Display();
  YAML::Node YAMLSerialize() override;
};

class ParamGroup : public Param<int> {
 public:
  void Display();

  void DisplayOptions();
  YAML::Node YAMLSerialize() override;

  void YAMLDeserialize(YAML::Node yaml_node) override { /* do nothing ?! */ }

 public:
  std::vector<std::shared_ptr<NodeParam>> params;
};

// utils
template <typename T>
T get_param_value(std::shared_ptr<NodeParam> param) {
  return static_cast<Param<T>*>(param.get())->Eval();
}
template <typename T>
void set_param_value(std::shared_ptr<NodeParam> param, T value) {
  std::dynamic_pointer_cast<Param<T>>(param)->Set(value);
};

};  // namespace NED
#endif