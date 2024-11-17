#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once
#include "ImGuiNode.h"
namespace NED::Utils {

struct FileFilterItem {
  std::string description;
  std::string extension;
};

std::shared_ptr<NED::AbstractNode> FindNodeByUUID(std::string uuid,
                                                         std::vector<std::shared_ptr<NED::AbstractNode>> nodes);

ImVec2 get_nodes_center(std::vector<std::shared_ptr<AbstractNode>>& nodes);
void deselect_all(std::vector<std::shared_ptr<AbstractNode>>& nodes);

std::filesystem::path open_file_explorer(std::vector<FileFilterItem> filters = {{"All Files", "*.*"}});
const std::wstring create_windows_file_explorer_Filter(const std::vector<FileFilterItem>& filters);
};  // namespace NodeEditor::Utils
#endif  // NODE_EDITOR_NODE_MANAGER_UTILS_H