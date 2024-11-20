#include "node_manager_utils.h"

namespace NED::Utils {
std::shared_ptr<AbstractNode> FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<AbstractNode>> nodes) {
  for (auto node : nodes) {
    if (node->uuid == uuid) {
      return node;
    }
  }
  return nullptr;
}

ImVec2 get_nodes_center(std::vector<std::shared_ptr<AbstractNode>>& nodes) {
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
void deselect_all(std::vector<std::shared_ptr<AbstractNode>>& nodes) {
  for (auto node : nodes) {
    node->selected = false;
  }
  // return ImVec2();
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
std::filesystem::path open_file_explorer(std::vector<FileFilterItem> filters) {
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
    std::string converted = wide_to_utf8(result_path.string());
    return std::filesystem::path(converted);  // Return the selected file path
  } else {
      std::cerr << "Failed to open file picker!" << std::endl;
      return std::filesystem::path("");  // Return empty path on failure
  }
#else
  return std::filesystem::path("");
#endif
}
};  // namespace NodeEditor::Utils