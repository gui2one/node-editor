#include "NodeParam.h"

namespace NED {
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
};  // namespace NodeEditor
