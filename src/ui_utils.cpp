#include "ui_utils.h"

namespace NED::UI {
bool Begin(const char* name, bool* p_open, ImGuiWindowFlags flags) {
  ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
  // flags |= ImGuiWindowFlags_NoTitleBar;
  return ImGui::Begin(name, p_open, flags);
}
void End() { ImGui::End(); };

void DisplayParamModel(std::shared_ptr<NED::NodeParam> param, std::function<void()> func) {
  ImGui::PushID(param->m_Label);
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 150.f);
  ImVec2 marker_size = ImVec2(5, 0);  // 0 height means not set
  if (param->IsDefault()) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.05f));  // invisible ?
    ImGui::BeginDisabled();
    ImGui::Button("##already_at_default", marker_size);
    ImGui::EndDisabled();
    ImGui::PopStyleColor();

  } else {
    if (ImGui::Button("##set_to_default", marker_size)) {
      param->SetToDefault();
    }
  }
  ImGui::SameLine();
  ImGui::Text("%s", param->m_Label);
  ImGui::NextColumn();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
  func();
  ImGui::PopItemWidth();
  ImGui::Columns(1);
  ImGui::PopID();
  ImGui::Spacing();
}

}  // namespace NED::UI