#ifndef PARAMS_H
#define PARAMS_H

#include <imgui.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "NodeParam.h"

namespace YAML {};  // namespace YAML

namespace NED {

template <>
class Param<double> : public NodeParam {
 public:
  double value;
  double temp_value;
  double old_value;
  // double min_val = std::numeric_limits<double>::min();
  // double max_val = std::numeric_limits<double>::max();
  double min_val = 0.0;
  double max_val = 300.0;

 public:
  Param() = default;
  // Param(const char* _name) : NodeParam(_name), value(0.0) {};
  Param(const char* _name, double _value) : NodeParam(_name), value(_value), temp_value(_value), old_value(_value) {}
  ~Param() {}

  double Eval() { return value; }
  void Set(double _value) {
    value = _value;
    temp_value = _value;
  }

  void Display() override {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      ImGuiSliderFlags flags = 0;
      if (ImGui::SliderScalar("##_label", ImGuiDataType_Double, &temp_value, (const void*)&min_val,
                              (const void*)&max_val)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(double, m_Node, m_Label, this->value, this->old_value);
      }
    });
  }
  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<double>()); }
};
};  // namespace NED
#endif