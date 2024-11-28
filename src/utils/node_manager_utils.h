#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once
#include "ImGuiNode.h"

namespace NED {

// forward declare
class NodeParam;

struct FileFilterItem {
  std::string description;
  std::string extension;
};

std::shared_ptr<AbstractNode> Utils::FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<AbstractNode>> nodes);

std::shared_ptr<NodeParam> Utils::FindParamByName(std::shared_ptr<AbstractNode> factory_node, std::string param_name);
ImVec2 Utils::get_nodes_center(std::vector<std::shared_ptr<AbstractNode>>& nodes);
void Utils::deselect_all(std::vector<std::shared_ptr<AbstractNode>>& nodes);

std::filesystem::path Utils::open_file_explorer(std::vector<FileFilterItem> filters = {{"All Files", "*.*"}});
const std::wstring Utils::create_windows_file_explorer_Filter(const std::vector<FileFilterItem>& filters);
};  // namespace NED
#endif  // NODE_EDITOR_NODE_MANAGER_UTILS_H