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

 public:
  Param() = default;
  Param(const char* _name) : NodeParam(_name), value(0.0) {};
  Param(const char* _name, double _value) : NodeParam(_name), value(_value) {}
  ~Param() {}
  void Set(double _value) {
    value = _value;
    temp_value = _value;
  }

  void Display() override {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() { ImGui::InputDouble(m_Label, &value); });
  }
  NODE_EDITOR_PARAM_YAML_SERIALIZE_FUNC();
  void YAMLDeserialize(YAML::Node yaml_node) override { Set(yaml_node["value"].as<double>()); }
};
};  // namespace NED
#endif