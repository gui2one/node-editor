#ifndef PARAMS_H
#define PARAMS_H

#include <imgui.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "NodeParam.h"

namespace YAML {};  // namespace YAML

namespace NED {

class ParamDouble : public Param<double> {
 public:
  ParamDouble() {
    min_val = 0.0;
    max_val = 1.0;
  }
  void Display() override {
    DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
      ImGuiSliderFlags flags = 0;
      if (ImGui::SliderScalar("##_label", ImGuiDataType_Double, &temp_value, (const void*)&min_val,
                              (const void*)&max_val, "%f", flags)) {
      }
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        this->old_value = this->value;
        this->value = this->temp_value;
        DISPATCH_PARAM_CHANGE_EVENT(double, m_Node, m_Label, this->value, this->old_value);
      }
    });
  }
  void DisplayOptions() override {
    ImGui::InputDouble("min", &min_val);
    ImGui::InputDouble("max", &max_val);
  }
};
};  // namespace NED
#endif