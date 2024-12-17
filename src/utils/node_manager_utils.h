#ifndef NODE_EDITOR_NODE_MANAGER_UTILS_H
#define NODE_EDITOR_NODE_MANAGER_UTILS_H
#pragma once
#include "ImGuiNode.h"

namespace NED {

// forward declare
class NodeParam;
namespace Utils {
struct FileFilterItem {
  std::string description;
  std::string extension;
};

std::shared_ptr<AbstractNode> FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<AbstractNode>> nodes);

std::shared_ptr<NodeParam> FindParamByName(AbstractNode* factory_node, std::string param_name);
ImVec2 get_nodes_center(std::vector<std::shared_ptr<AbstractNode>>& nodes);
void deselect_all(std::vector<std::shared_ptr<AbstractNode>>& nodes);

std::filesystem::path open_file_explorer(std::vector<FileFilterItem> filters = {{"All Files", "*.*"}},
                                         bool save_mode = false);
const std::wstring create_windows_file_explorer_Filter(const std::vector<FileFilterItem>& filters);
}  // namespace Utils
};  // namespace NED
#endif  // NODE_EDITOR_NODE_MANAGER_UTILS_H