#ifndef NODE_EDITOR_UI_UTILS_H
#define NODE_EDITOR_UI_UTILS_H
#pragma once

#include <imgui.h>

namespace NED::UI {
bool Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);

void End();
};  // namespace NED::UI
#endif  // NODE_EDITOR_UI_UTILS_H