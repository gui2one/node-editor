#ifndef NODEPARAM_H
#define NODEPARAM_H
#pragma once

#include <imgui_internal.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Event.h"
#include "EventManager.h"
#include "utils.h"
#include "utils/node_manager_utils.h"

#define NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC()                                     \
  YAML::Node YAMLSerialize() override {                                             \
    YAML::Node yaml_node;                                                           \
    std::string type_str = NodeEditor::clean_param_type_name(typeid(*this).name()); \
    yaml_node["type"] = type_str;                                                   \
    yaml_node["name"] = name;                                                       \
    yaml_node["value"] = value;                                                     \
    return yaml_node;                                                               \
  }

#define DISPATCH_PARAM_CHANGE_EVENT() \
  ParamChangedEvent event;            \
  EventManager::GetInstance().Dispatch(event);
#define DISPATCH_EDITOR_UPDATE_EVENT() \
  ManagerUpdateEvent event;            \
  EventManager::GetInstance().Dispatch(event);

#define DISPLAY_PARAM_TEMPLATE(label, func)               \
  ImGui::PushID(name);                                    \
  ImGui::Columns(2);                                      \
  ImGui::SetColumnWidth(0, 150.f);                        \
  ImGui::Text("%s", name);                                \
  ImGui::NextColumn();                                    \
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); \
  func();                                                 \
  ImGui::PopItemWidth();                                  \
  ImGui::Columns(1);                                      \
  ImGui::PopID();                                         \
  ImGui::Spacing();

namespace NodeEditor {

class NodeParam {
 public:
  NodeParam() { name = "no name"; };
  NodeParam(const char* _name) : name(_name) {}
  virtual ~NodeParam() = default;
  virtual void Display() = 0;

  virtual YAML::Node YAMLSerialize() = 0;

 public:
  const char* name;
  int value = -1;
};

template <typename T>
class Param : public NodeParam {
 public:
  Param() : NodeParam() {};
  Param(const char* _name, T _value, T _min_val, T _max_val, T _default_val)
      : NodeParam(_name), value(_value), min_val(_min_val), max_val(_max_val), default_val(_default_val) {};
  ~Param() {};

  T Eval() { return value; };

  void Display() { ImGui::Text("%s -- not implemented", name); }

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    yaml_node["type"] = typeid(*this).name();
    yaml_node["name"] = name;
    yaml_node["value"] = value;
    return yaml_node;
  }

 public:
  T value;
  T min_val;
  T max_val;
  T default_val;

 private:
};

template <>
class Param<glm::vec2> : public NodeParam {
 public:
  Param(): NodeParam() {};
  Param(const char* _name, glm::vec2 _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  glm::vec2 Eval() { return value; }

  void Display(){DISPLAY_PARAM_TEMPLATE(name, [this]() {;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.8f, 0.1f, 0.1f, 1.f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.9f, 0.1f, 0.1f, 1.f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(1.0f, 0.1f, 0.1f, 1.f)));

    float avail_x = ImGui::GetContentRegionAvail().x;
    float spacing = ImGui::GetCurrentContext()->Style.ItemInnerSpacing.x;
    float input_width = avail_x / 2.0f - 30.0f - spacing;
    ImGui::PushID(0);
    if (ImGui::Button("X", ImVec2(30, 25))) {
      value.x = default_val.x;
      DISPATCH_PARAM_CHANGE_EVENT();
    }
    ImGui::PopID();
    ImGui::PopStyleColor(3);
    ImGui::SameLine();

    ImGui::PushID(1);
    ImGui::PushItemWidth(input_width);
    if (ImGui::DragFloat("##x", &value.x, 0.05f)) {
      DISPATCH_PARAM_CHANGE_EVENT();
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.5f, 0.1f, 1.f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.6f, 0.1f, 1.f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.7f, 0.1f, 1.f)));
    ImGui::PushID("label_y");

    if (ImGui::Button("Y", ImVec2(30, 25))) {
      value.y = default_val.y;
      DISPATCH_PARAM_CHANGE_EVENT();
    }
    ImGui::PopID();
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::PushID("float_y");
    ImGui::PushItemWidth(input_width);
    if (ImGui::DragFloat("##y", &value.y, 0.05f)) {
      DISPATCH_PARAM_CHANGE_EVENT();
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
  })} NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

 public:
  glm::vec2 value;
  glm::vec2 default_val = glm::vec2(0.0f);
};

template <>
class Param<glm::vec3> : public NodeParam {
 public:
  Param(): NodeParam() {};
  Param(const char* _name, glm::vec3 _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  glm::vec3 Eval() { return value; }

  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      // float default_value = 0.0f;

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.8f, 0.1f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.9f, 0.1f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(1.0f, 0.1f, 0.1f, 1.f)));

      float avail_x = ImGui::GetContentRegionAvail().x;
      float spacing = ImGui::GetCurrentContext()->Style.ItemInnerSpacing.x;
      float input_width = avail_x / 3.0f - 30.0f - spacing;
      ImGui::PushID(0);
      if (ImGui::Button("X", ImVec2(30, 25))) {
        value.x = default_val.x;
        DISPATCH_PARAM_CHANGE_EVENT();
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();

      ImGui::PushID(1);
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##x", &value.x, 0.05f)) {
        DISPATCH_PARAM_CHANGE_EVENT();
      }
      ImGui::PopID();
      ImGui::PopItemWidth();

      ImGui::SameLine(0, spacing);

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.5f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.6f, 0.1f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.7f, 0.1f, 1.f)));
      ImGui::PushID("label_y");

      if (ImGui::Button("Y", ImVec2(30, 25))) {
        value.y = default_val.y;
        DISPATCH_PARAM_CHANGE_EVENT();
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::PushID("float_y");
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##y", &value.y, 0.05f)) {
        DISPATCH_PARAM_CHANGE_EVENT();
      }
      ImGui::PopID();
      ImGui::PopItemWidth();

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0.1f, 0.1f, 0.8f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0.1f, 0.1f, 0.9f, 1.f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0.1f, 0.1f, 1.0f, 1.f)));
      ImGui::SameLine(0, spacing);
      ImGui::PushID("label_z");
      if (ImGui::Button("Z", ImVec2(30, 25))) {
        value.z = default_val.z;
        DISPATCH_PARAM_CHANGE_EVENT();
      }
      ImGui::PopID();
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::PushID("float_z");
      ImGui::PushItemWidth(input_width);
      if (ImGui::DragFloat("##z", &value.z, 0.05f)) {
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
  glm::vec3 default_val = glm::vec3(0.0f);
};

template <>
class Param<std::string> : public NodeParam {
public:
  Param(): NodeParam() {};
  Param(const char* _name) : NodeParam(_name), value("no value") {};
  Param(const char* _name, std::string _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  std::string Eval() { return value; }

  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      char buffer[2048];
      if (value.length() > 2048) value = value.substr(0, 2048);
      std::copy(value.begin(), value.end(), buffer);
      buffer[value.length()] = 0;
      if (ImGui::InputText("##name", buffer, 2048)) {
        value = std::string(buffer);
        DISPATCH_PARAM_CHANGE_EVENT();
      }
    });
  }

  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

 public:
  std::string value;
};
template <>
class Param<std::wstring> : public NodeParam {
 public:
  Param() : NodeParam() {};
  Param(const char* _name, std::wstring _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  std::wstring Eval() { return value; }

  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      std::string converted = wide_to_utf8(value);
      char buffer[2048];
      if (converted.length() > 2048) converted = converted.substr(0, 2048);
      std::copy(converted.begin(), converted.end(), buffer);
      buffer[converted.length()] = 0;
      if (ImGui::InputText("##name", (char*)buffer, 2048)) {
        // value = std::wstring(buffer);
        DISPATCH_PARAM_CHANGE_EVENT();
      }
    });
  }

  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

 public:
  std::wstring value;
};

template <>
class Param<int> : public NodeParam {
 public:
  Param(): NodeParam() {};
  Param(const char* _name, int _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  int Eval() { return value; }

  void Display(){DISPLAY_PARAM_TEMPLATE(name, [this]() {
    if (ImGui::DragInt("##name", &value, 1.0f, min_val, max_val, "%d", ImGuiSliderFlags_AlwaysClamp)) {
      DISPATCH_PARAM_CHANGE_EVENT();
    }
  })} NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

 public:
  int value;
  int min_val = INT_MIN;
  int max_val = INT_MAX;
};

template <>
class Param<float> : public NodeParam {
 public:
  Param(): NodeParam() {};
  Param(const char* _name, float _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  float Eval() { return value; }

  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      if (ImGui::SliderFloat("##name", &value, 0, 100)) {
        DISPATCH_PARAM_CHANGE_EVENT();
      }
    });
  }
  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

 public:
  float value;
};

template <>
class Param<bool> : public NodeParam {
 public:
  Param(): NodeParam() {};
  Param(const char* _name, bool _value) : NodeParam(_name), value(_value) {};
  ~Param() {};

  bool Eval() { return value; }

  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      if (ImGui::Checkbox("##name", &value)) {
        DISPATCH_PARAM_CHANGE_EVENT();
      }
    });
  }
  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();

 public:
  bool value;
};

class ParamFile : public Param<std::wstring> {
 public:
  std::vector<Utils::FileFilterItem> filters = {{"All Files", "*"}, {"Text Files", "txt"}};

 public:
  ParamFile() : Param<std::wstring>() {};
  ParamFile(const char* _name, std::wstring _value) : Param(_name, _value) {};
  ~ParamFile() {};

  std::wstring Eval() { return value; }
  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      // ImGui::Text("ParamFile Not implemented yet ...");
      float avail_x = ImGui::GetContentRegionAvail().x;
      ImGui::PushItemWidth(avail_x - 50.0f);

      // ImGui::InputText("##name", &value);
      std::string converted = wide_to_utf8(value);
      ImGui::InputText("##name", &converted, 2048);

      ImGui::PopItemWidth();
      ImGui::SameLine();
      ImGui::PushItemWidth(50.0f);
      if (ImGui::Button("Browse")) {
        auto path = Utils::open_file_explorer(filters);
        if (path != "") {
          value = path.wstring();
          DISPATCH_PARAM_CHANGE_EVENT();
        }
      }
      ImGui::PopItemWidth();
    });
  }
  // unable to use the MACRO here because of wide_to_utf8() call
  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = clean_param_type_name(typeid(*this).name());
    yaml_node["type"] = type_str;
    yaml_node["name"] = name;
    yaml_node["value"] = wide_to_utf8(value);
    return yaml_node;
  }
};


class ParamLabel : public Param<std::string> {
 public:
  ParamLabel() : Param<std::string>("aaaa", "no value"){};
  ParamLabel(std::string _value) : Param<std::string>("", _value){};
  ParamLabel(const char* _name, std::string _value) : Param<std::string>(_name, _value){};
  ~ParamLabel() {};
  void Display() {
    ImGui::Spacing();
    ImGui::Text("%s", value.c_str());
  }
  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = clean_param_type_name(typeid(*this).name());
    yaml_node["type"] = type_str;
    yaml_node["name"] = name;
    yaml_node["value"] = value;
    return yaml_node;
  }

};

class ParamComboBox : public Param<int> {
 public:
  ParamComboBox() : Param<int>("name", 0) {};
  ParamComboBox(const char* _name) : Param<int>(_name, 0) {};
  ~ParamComboBox() {};
  void Display() {
    DISPLAY_PARAM_TEMPLATE(name, [this]() {
      if (ImGui::Combo("##name", &value, choices.data(), static_cast<int>(choices.size()))) {
        DISPATCH_PARAM_CHANGE_EVENT();
      }
    })
  }

  inline int GetChoice() { return value; }
  inline void SetChoice(int choice) { value = choice; }
  inline void SetChoices(std::vector<const char*> _choices) { choices = _choices; }

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = clean_param_type_name(typeid(*this).name());
    yaml_node["type"] = type_str;
    yaml_node["name"] = name;
    yaml_node["value"] = value;
    return yaml_node;
  }

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
    std::string type_str = clean_param_type_name(typeid(*this).name());
    yaml_node["type"] = type_str;
    yaml_node["name"] = name;
    yaml_node["value"] = "null";
    return yaml_node;
  }
};


class ParamGroup : public Param<int> {
 public:
  ParamGroup() : Param<int>("", 0) {};
  ParamGroup(const char* _name) : Param<int>(_name, 0) {};
  ~ParamGroup() {};
  void Display() override;

  YAML::Node YAMLSerialize() override {
    YAML::Node yaml_node;
    std::string type_str = clean_param_type_name(typeid(*this).name());
    yaml_node["type"] = type_str;
    yaml_node["name"] = name;

    for (auto item : params) {
      yaml_node["params"].push_back(item->YAMLSerialize());
    }
    yaml_node["value"] = "null";
    return yaml_node;
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
  static_cast<Param<T>*>(param.get())->value = value;
};

};  // namespace NodeEditor
#endif