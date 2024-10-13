#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once

// #include <imgui.h>
namespace NodeEditor::Utils{

    ImVec2 to_canvas_space(ImVec2 pos, ImVec2 origin, float zoom);
};
#endif // NODE_EDITOR_NODE_MANAGER_UTILS_H