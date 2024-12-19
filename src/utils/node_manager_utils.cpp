#include "node_manager_utils.h"

#include "NodeParam.h"
namespace NED::Utils {
std::shared_ptr<AbstractNode> FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<AbstractNode>> nodes) {
  for (auto node : nodes) {
    if (node->uuid == uuid) {
      return node;
    }
  }
  return nullptr;
}

std::shared_ptr<NED::NodeParam> FindParamByName(AbstractNode* factory_node, std::string param_name) {
  for (auto& param_item : factory_node->m_ParamLayout.params) {
    auto p_group = std::dynamic_pointer_cast<NED::ParamGroup>(param_item);
    if (p_group != nullptr) {
      if (p_group->m_Label == param_name) {
        return p_group;
      }
      for (auto group_item : p_group->params) {
        if (group_item->m_Label == param_name) {
          return group_item;
        }
      }

    } else {
      if (param_item->m_Label == param_name) {
        return param_item;
      }
    }
  }

  return nullptr;
}

ImVec2 get_nodes_center(std::vector<std::shared_ptr<NED::AbstractNode>>& nodes) {
  if (nodes.size() == 0) return ImVec2(0, 0);
  float minx = 999999999.f, miny = 999999999.f, maxx = -999999999.f, maxy = -999999999.f;

  for (auto node : nodes) {
    if (node->position.x < minx) minx = node->position.x;
    if (node->position.y < miny) miny = node->position.y;
    if (node->position.x > maxx) maxx = node->position.x;
    if (node->position.y > maxy) maxy = node->position.y;
  }
  float centerx, centery;
  centerx = (minx + maxx) / 2.0f + nodes[0]->size.x / 2.0f;
  centery = (miny + maxy) / 2.0f + nodes[0]->size.y / 2.0f;
  return ImVec2(centerx, centery);
}
void deselect_all(std::vector<std::shared_ptr<NED::AbstractNode>>& nodes) {
  for (auto node : nodes) {
    node->selected = false;
  }
  // return ImVec2();
}

bool compare_selections(std::vector<AbstractNode*> selA, std::vector<AbstractNode*> selB) {
  if (selA.size() != selB.size()) return false;
  // sort nodes by uuid
  std::sort(selA.begin(), selA.end(), [](AbstractNode* a, AbstractNode* b) { return a->uuid < b->uuid; });
  std::sort(selB.begin(), selB.end(), [](AbstractNode* a, AbstractNode* b) { return a->uuid < b->uuid; });
  for (int i = 0; i < selA.size(); i++) {
    if (selA[i]->uuid != selB[i]->uuid) return false;
  }
  return true;
}

bool rectOverlap(Rect A, Rect B) {
  bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) || valueInRange(B.x, A.x, A.x + A.width);

  bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) || valueInRange(B.y, A.y, A.y + A.height);

  return xOverlap && yOverlap;
}

Rect selection_rect(ImVec2 start, ImVec2 end) {
  Utils::Rect selection_rect;

  if (start.x < end.x) {
    selection_rect.x = start.x;
    selection_rect.width = std::abs(end.x - start.x);
  } else if (start.x > end.x) {
    selection_rect.width = std::abs(start.x - end.x);
    selection_rect.x = end.x;
  } else {
    selection_rect.x = start.x;
    selection_rect.width = 1.0f;
  }

  if (start.y < end.y) {
    selection_rect.y = start.y;
    selection_rect.height = std::abs(end.y - start.y);
  } else if (start.y > end.y) {
    selection_rect.height = std::abs(start.y - end.y);
    selection_rect.y = end.y;
  } else {
    selection_rect.y = start.y;
    selection_rect.height = 1.0f;
  }

  return selection_rect;
}

bool valueInRange(float value, float min, float max) { return (value >= min) && (value <= max); }

bool node_in_rect(AbstractNode* node, Rect rect) {
  bool inside_x = false;
  bool inside_y = false;

  Utils::Rect node_rect;
  node_rect.x = node->position.x;
  node_rect.y = node->position.y;
  node_rect.width = node->size.x;
  node_rect.height = node->size.y;

  return rectOverlap(node_rect, rect);
}

const std::wstring create_windows_file_explorer_Filter(const std::vector<FileFilterItem>& filters) {
  std::wstring filter;

  for (const auto& item : filters) {
    // Append description followed by a null character
    auto desc_w = utf8_to_wide(item.description);
    auto ext_w = utf8_to_wide(item.extension);

    filter.append(desc_w);
    filter.push_back(L'\0');

    // Append pattern with a wildcard * and a null character
    filter.append(L"*.");
    filter.append(ext_w);
    filter.push_back(L'\0');
  }

  // Append an additional null character to terminate the filter
  filter.push_back(L'\0');

  return filter;
}
std::filesystem::path open_file_explorer(std::vector<FileFilterItem> filters, bool save_mode) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  OPENFILENAMEW ofn;
  wchar_t szFile[260] = L"";
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = 260;
  ofn.lpstrTitle = L"Select a file";
  std::wstring filter = create_windows_file_explorer_Filter(filters);
  ofn.lpstrFilter = filter.c_str();
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_CREATEPROMPT;
  if (save_mode) {
    ofn.Flags |= OFN_OVERWRITEPROMPT;
  }

  if (GetSaveFileNameW(&ofn) == TRUE)  // Changed to GetOpenFileNameW
  {
    auto result_path = std::filesystem::path(ofn.lpstrFile);
    std::string converted = wide_to_utf8(result_path.wstring());
    std::cout << "File Selected : " << converted << std::endl;

    return result_path;
  } else {
    return std::filesystem::path(L"");
  }
#elif __linux__
  // Command to open file dialog using zenity
  std::string command = "zenity --file-selection --title='Select a file' --file-filter='*.*'";

  // Execute the command and capture the result
  char buffer[128];
  std::string result = "";
  FILE* fp = popen(command.c_str(), "r");

  if (fp) {
    while (fgets(buffer, sizeof(buffer), fp)) {
      result += buffer;
    }
    fclose(fp);

    // Remove any trailing newlines or extra spaces
    if (!result.empty() && result[result.size() - 1] == '\n') {
      result.erase(result.size() - 1);
    }

    std::cout << "File Selected: " << result << std::endl;
    auto result_path = std::filesystem::path(result);
    // std::string converted = wide_to_utf8(result_path.string());
    return std::filesystem::path(result_path.string());  // Return the selected file path
  } else {
    std::cerr << "Failed to open file picker!" << std::endl;
    return std::filesystem::path("");  // Return empty path on failure
  }
#else
  return std::filesystem::path("");
#endif
}
};  // namespace NED::Utils