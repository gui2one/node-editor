#include "ui_utils.h"

namespace NED::UI {
bool Begin(const char* name, bool* p_open, ImGuiWindowFlags flags) {
  ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
  // flags |= ImGuiWindowFlags_NoTitleBar;
  return ImGui::Begin(name, p_open, flags);
}
void End() { ImGui::End(); };

}  // namespace NED::UI