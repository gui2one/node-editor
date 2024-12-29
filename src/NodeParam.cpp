#include "NodeParam.h"

namespace NED {

void ParamFloat::Display() {
  DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
    ImGuiSliderFlags flags = 0;

    if (ImGui::SliderFloat("##m_Label", &temp_value, min_val, max_val, "%.3f", flags)) {
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
      old_value = value;
      value = temp_value;
      DISPATCH_PARAM_CHANGE_EVENT(float, m_Node, m_Label, value, old_value);
    }
  });
}
void ParamFloat::DisplayOptions() {
  ImGui::PushID(this);
  ImGui::InputFloat("min", &min_val);
  ImGui::InputFloat("max", &max_val);
  ImGui::PopID();
}

void ParamInt::Display() {
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
void ParamInt::DisplayOptions() {
  ImGui::PushID(this);
  ImGui::InputInt("min", &min_val);
  ImGui::InputInt("max", &max_val);
  ImGui::PopID();
}

void ParamBool::Display() {
  DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
    if (ImGui::Checkbox("##m_Label", &temp_value)) {
      this->old_value = this->value;
      this->value = this->temp_value;
      DISPATCH_PARAM_CHANGE_EVENT(bool, m_Node, m_Label, this->value, this->old_value);
    }
  });
}

void ParamString::Display() {
  DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
    ImGuiInputTextFlags flags = 0;

    if (ImGui::InputText("##m_Label", &temp_value, flags)) {
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
      this->old_value = this->value;
      this->value = this->temp_value;
      DISPATCH_PARAM_CHANGE_EVENT(std::string, m_Node, m_Label, this->value, this->old_value);
    }
  });
}

void ParamVec2::Display() {
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
      temp_value.x = default_val.x;
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
      temp_value.y = default_val.y;
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

void ParamVec3::Display() {
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
      temp_value.x = default_val.x;
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
      temp_value.y = default_val.y;
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
      temp_value.z = default_val.z;
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

void ParamColor::Display() {
  DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
    ImGuiColorEditFlags flags = 0;
    flags |= ImGuiColorEditFlags_Float;
    if (ImGui::ColorEdit3("##color", glm::value_ptr(temp_value), flags)) {
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
      this->old_value = this->value;
      this->value = this->temp_value;
      DISPATCH_PARAM_CHANGE_EVENT(glm::vec3, m_Node, m_Label, value, old_value);
    }
  });
}

void ParamFile::Display() {
  DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
    float avail_x = ImGui::GetContentRegionAvail().x;
    ImGui::PushItemWidth(avail_x - 50.0f);

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
YAML::Node ParamFile::YAMLSerialize() {
  YAML::Node yaml_node;
  std::string type_str = std::string(m_TypeName);
  yaml_node["type"] = type_str;
  yaml_node["label"] = m_Label;
  yaml_node["value"] = wide_to_utf8(value);
  return yaml_node;
}

void ParamLabel::Display() {
  ImGui::Spacing();
  ImGui::Text("%s", value.c_str());
}

void ParamComboBox::Display() {
  DISPLAY_PARAM_TEMPLATE(m_Label, [this]() {
    if (ImGui::Combo("##m_Label", &temp_value, choices.data(), static_cast<int>(choices.size()))) {
      old_value = value;
      value = temp_value;
      DISPATCH_PARAM_CHANGE_EVENT(int, m_Node, m_Label, value, old_value);
    }
  })
}

void ParamSeparator::Display() {
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
}
YAML::Node ParamSeparator::YAMLSerialize() {
  YAML::Node yaml_node;
  std::string type_str = std::string(m_TypeName);
  yaml_node["type"] = type_str;
  yaml_node["label"] = m_Label;
  yaml_node["value"] = "null";
  return yaml_node;
}

void ParamGroup::Display() {
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
}
void ParamGroup::DisplayOptions() {
  for (auto param : params) {
    param->DisplayOptions();
  }
}
YAML::Node ParamGroup::YAMLSerialize() {
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

}  // namespace NED