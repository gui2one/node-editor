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
  ManagerUpdateEvent event;            \
  EventManager::GetInstance().Dispatch(event);

#define DISPLAY_PARAM_TEMPLATE(label, func)               \
  ImGui::PushID(m_Label);                                 \
  ImGui::Columns(2);                                      \
  ImGui::SetColumnWidth(0, 150.f);                        \
  ImGui::Text("%s", m_Label);                             \
  ImGui::NextColumn();                                    \
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); \
  func();                                                 \
  ImGui::PopItemWidth();                                  \
  ImGui::Columns(1);                                      \
  ImGui::PopID();                                         \
  ImGui::Spacing();

namespace NED {
// forward declaree
class AbstractNode;
class NodeParam {
 public:
  NodeParam() : m_Label("no label"), m_TypeName("no type") {}
  NodeParam(const char* _name) : m_Label(_name), m_TypeName("no type") {}
  virtual ~NodeParam() = default;
  virtual void Display() = 0;

  virtual YAML::Node YAMLSerialize() = 0;
  virtual void YAMLDeserialize(YAML::Node yaml_node) = 0;

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

  void Set(T _value) {
    temp_value = _value;
    value = _value;
  };

  void Display() { ImGui::Text("%s -- not implemented", m_Label); }

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    yaml_node["type"] = std::string(m_TypeName);
    yaml_node["label"] = m_Label;
    yaml_node["value"] = value;
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

template <>
class Param<std::string> : public NodeParam {
 public:
  Param() = default;
  Param(const char* _name) : NodeParam(_name), value("no value") {};
  Param(const char* _name, std::string _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  std::string Eval() { return value; }
  void Set(std::string _value) {
    value = _value;
    temp_value = _value;
  }
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      ImGuiInputTextFlags flags = 0;
      // flags |= ImGuiInputTextFlags_EnterReturnsTrue;

      if (ImGui::InputText("##m_Label", &temp_value, flags)) {
      }

      if (ImGui::IsItemDeactivatedAfterEdit()) {
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(std::string, m_Node, m_Label, this->value, this->old_value);
      }
    });
  }

  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<std::string>()); }

 public:
  std::string value;
  std::string old_value;
  std::string temp_value;
};
template <>
class Param<std::wstring> : public NodeParam {
 public:
  Param() = default;
  Param(const char* _name, std::wstring _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  std::wstring Eval() { return value; }
  void Set(std::wstring _value) {
    value = _value;
    temp_value = _value;
  }
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      std::string converted = wide_to_utf8(value);
      if (ImGui::InputText("##m_Label", &converted)) {
        this->temp_value = utf8_to_wide(converted);
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(std::wstring, m_Node, m_Label, value, old_value);
      }
    });
  }

  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<std::wstring>()); }

 public:
  std::wstring value;
  std::wstring old_value;
  std::wstring temp_value;
};

// template <>
// class Param<int> : public NodeParam {
//  public:
//   Param() = default;
//   Param(const char* _name, int _value) : NodeParam(_name), value(_value), temp_value(_value), old_value(_value) {};
//   ~Param() {};
//
//   int Eval() { return value; }
//   void Set(int _value) {
//     value = _value;
//     temp_value = _value;
//   }
//   void Display() {
//     DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
//       ImGuiSliderFlags flags = 0;
//       flags |= ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput;
//       if (ImGui::DragInt("##m_Label", &temp_value, 1.0f, min_val, max_val, "%d", flags)) {
//       }
//       if (ImGui::IsItemDeactivatedAfterEdit()) {
//         this->old_value = this->value;
//         this->value = this->temp_value;
//         DISPATCH_PARAM_CHANGE_EVENT(int, m_Node, m_Label, this->value, this->old_value);
//       }
//     });
//   }
//
//   NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
//   void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<int>()); }
//
//  public:
//   int value = 0;
//   int old_value = 0;
//   int temp_value = 0;
//   int min_val = std::numeric_limits<int>::min();
//   int max_val = std::numeric_limits<int>::max();
// };

template <>
class Param<bool> : public NodeParam {
 public:
  Param() = default;
  Param(const char* _name, bool _value) : NodeParam(_name), value(_value), temp_value(_value), old_value(_value) {};
  ~Param() {};

  bool Eval() { return value; }
  void Set(bool _value) {
    value = _value;
    temp_value = _value;
  }
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      if (ImGui::Checkbox("##m_Label", &temp_value)) {
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(bool, m_Node, m_Label, this->value, this->old_value);
      }
    });
  }
  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<bool>()); }

 public:
  bool value = false;
  bool old_value = false;
  bool temp_value = false;
};

/* derived classes */
class ParamFloat : public Param<float> {
 public:
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      ImGuiSliderFlags flags = 0;

      if (ImGui::SliderFloat("##m_Label", &temp_value, 0, 100, "%.3f", flags)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        old_value = value;
        value = temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(float, m_Node, m_Label, value, old_value);
      }
    });
  }
};
class ParamInt : public Param<int> {
 public:
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      ImGuiSliderFlags flags = 0;
      flags |= ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput;
      if (ImGui::DragInt("##m_Label", &temp_value, 1.0f, min_val, max_val, "%d", flags)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(int, m_Node, m_Label, this->value, this->old_value);
      }
    });
  }
};
class ParamVec3 : public Param<glm::vec3> {
 public:
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      bool value_changed = false;

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.8f, 0.1f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.9f, 0.1f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(1.0f, 0.1f, 0.1f, 1.f)));

      float avail_x = ImGui::GetContentRegionAvail().x;
      float spacing = ImGui::GetCurrentContext()->Style.ItemInnerSpacing.x;
      float input_width = avail_x / 3.0f - 30.0f - spacing;
      ImGui::PushID(0);
      if (ImGui::Button("X", ImVec2(30, 25))) {
        value_changed = true;
        value.x = default_val.x;
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();

      ImGui::PushID(1);
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##x", &temp_value.x, 0.05f)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        value_changed = true;
      }
      ImGui::PopID();
      ImGui::PopItemWidth();

      ImGui::SameLine(0, spacing);

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.5f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.6f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.7f, 0.1f, 1.f)));
      ImGui::PushID("label_y");

      if (ImGui::Button("Y", ImVec2(30, 25))) {
        value_changed = true;
        value.y = default_val.y;
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::PushID("float_y");
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##y", &temp_value.y, 0.05f)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        value_changed = true;
      }
      ImGui::PopID();
      ImGui::PopItemWidth();

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.1f, 0.8f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.1f, 0.9f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.1f, 1.0f, 1.f)));
      ImGui::SameLine(0, spacing);
      ImGui::PushID("label_z");
      if (ImGui::Button("Z", ImVec2(30, 25))) {
        value_changed = true;
        value.z = default_val.z;
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::PushID("float_z");
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##z", &temp_value.z, 0.05f)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        value_changed = true;
      }
      ImGui::PopID();
      ImGui::PopItemWidth();
      ImGui::PopStyleVar();

      if (value_changed) {
        // std::cout << "Vec3 value changed " << this->value.x << ", " << this->value.y << std::endl;
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(glm::vec3, m_Node, m_Label, value, old_value);
      }
    });
  }
};
class ParamVec2 : public Param<glm::vec2> {
 public:
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      bool value_changed = false;
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.8f, 0.1f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.9f, 0.1f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(1.0f, 0.1f, 0.1f, 1.f)));

      float avail_x = ImGui::GetContentRegionAvail().x;
      float spacing = ImGui::GetCurrentContext()->Style.ItemInnerSpacing.x;
      float input_width = avail_x / 2.0f - 30.0f - spacing;
      ImGui::PushID(0);
      if (ImGui::Button("X", ImVec2(30, 25))) {
        value_changed = true;
        value.x = default_val.x;
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();

      ImGui::PushID(1);
      ImGui::PushItemWidth(input_width);
      ImGuiInputFlags flags = 0;
      // flags |= ImGuiInputFlags_RepeatUntilRelease;

      if (ImGui::DragFloat("##x", &this->temp_value.x, 0.05f, 1.0f, 0.0f, "%.3f", flags)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        value_changed = true;
      }
      ImGui::PopID();
      ImGui::PopItemWidth();

      ImGui::SameLine(0, spacing);

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.5f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.6f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.7f, 0.1f, 1.f)));
      ImGui::PushID("label_y");

      if (ImGui::Button("Y", ImVec2(30, 25))) {
        value_changed = true;
        value.y = default_val.y;
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::PushID("float_y");
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##y", &temp_value.y, 0.05f, 1.0f, 0.0f, "%.3f", flags)) {
        // value_changed = true;
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        value_changed = true;
      }
      ImGui::PopID();
      ImGui::PopItemWidth();

      ImGui::PopStyleVar();
      if (value_changed) {
        // std::cout << "Vec2 value changed " << this->value.x << ", " << this->value.y << std::endl;
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(glm::vec2, m_Node, m_Label, value, old_value);
      }
    });
  }
};

class ParamFile : public Param<std::wstring> {
 public:
  std::vector<Utils::FileFilterItem> filters = {{"All Files", "*"}, {"Text Files", "txt"}};

 public:
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      // ImGui::Text("ParamFile Not implemented yet ...");
      float avail_x = ImGui::GetContentRegionAvail().x;
      ImGui::PushItemWidth(avail_x - 50.0f);

      // ImGui::InputText("##m_Label", &value);
      std::string converted = wide_to_utf8(temp_value);
      ImGui::InputText("##m_Label", &converted, 2048);

      ImGui::PopItemWidth();
      ImGui::SameLine();
      ImGui::PushItemWidth(50.0f);
      if (ImGui::Button("Browse")) {
        auto path = Utils::open_file_explorer(filters);
        if (path != "") {
          temp_value = path.wstring();
          old_value = value;
          value = temp_value;

          DISPATCH_PARAM_CHANGE_EVENT(std::wstring, m_Node, m_Label, value, old_value);
        }
      }
      ImGui::PopItemWidth();
    });
  }
  // unable to use the MACRO here because of wide_to_utf8() call
  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = std::string(m_TypeName);
    yaml_node["type"] = type_str;
    yaml_node["label"] = m_Label;
    yaml_node["value"] = wide_to_utf8(value);
    return yaml_node;
  }
};

class ParamLabel : public Param<std::string> {
 public:
  void Display() {
    ImGui::Spacing();
    ImGui::Text("%s", value.c_str());
  }
};

class ParamComboBox : public Param<int> {
 public:
  // ParamComboBox() : Param<int>("m_Label", 0) {};
  // ParamComboBox(const char* _name) : Param<int>(_name, 0) {};
  //~ParamComboBox() {};
  void Display() {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      if (ImGui::Combo("##m_Label", &temp_value, choices.data(), static_cast<int>(choices.size()))) {
        old_value = value;
        value = temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(int, m_Node, m_Label, value, old_value);
      }
    })
  }

  inline int GetChoice() { return value; }
  inline void SetChoice(int choice) { value = choice; }
  inline void SetChoices(std::vector<const char*> _choices) { choices = _choices; }

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = std::string(m_TypeName);
    yaml_node["type"] = type_str;
    yaml_node["label"] = m_Label;
    yaml_node["value"] = value;
    return yaml_node;
  }
  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<int>()); }

 public:
  std::vector<const char*> choices = {"first", "second", "third"};
};

class ParamSeparator : public Param<std::string> {
 public:
  ParamSeparator() : Param<std::string>("separator") {};
  ParamSeparator(const char* _name) : Param<std::string>(_name) {};
  ~ParamSeparator() {};
  void Display() {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
  }
  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = std::string(m_TypeName);
    yaml_node["type"] = type_str;
    yaml_node["label"] = m_Label;
    yaml_node["value"] = "null";
    return yaml_node;
  }
};

class ParamGroup : public Param<int> {
 public:
  void Display() {
    ImGui::Spacing();
    ImGui::Text("%s", m_Label);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Indent(10.0f);
    ImGui::BeginGroup();

    for (auto item : params) {
      item->Display();
    }

    ImGui::EndGroup();
    ImGui::Indent(-10.0f);
  };

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = std::string(m_TypeName);
    yaml_node["type"] = type_str;
    yaml_node["label"] = m_Label;

    for (auto item : params) {
      yaml_node["params"].push_back(item->YAMLSerialize());
    }
    yaml_node["value"] = "null";
    return yaml_node;
  }

  void YAMLDeserialize(YAML::Node yaml_node) override {
    // for (auto item : yaml_node["params"]) {
    //
    //   Set(yaml_node["value"].as<int>());
    // }
  }

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