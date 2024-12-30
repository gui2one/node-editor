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