#ifndef NODE_EDITOR_UI_UTILS_H
#define NODE_EDITOR_UI_UTILS_H
#pragma once

#include <imgui.h>

namespace NED::UI {
bool Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);

void End();

void DisplayParamModel(std::shared_ptr<NED::NodeParam> param, std::function<void()> func);
};  // namespace NED::UI
#endif  // NODE_EDITOR_UI_UTILS_H