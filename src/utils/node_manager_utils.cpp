#include "node_manager_utils.h"
#ifdef __linux__
// #include <unistd.h>
#include <gtk/gtk.h>
#endif
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
  // For Linux, use GTK for file dialog
  GtkWidget *dialog;
  gtk_init(0, nullptr); // Initialize GTK
  dialog = gtk_file_chooser_dialog_new("Select a file",
                                       NULL,
                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                       "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Open", GTK_RESPONSE_ACCEPT,
                                       NULL);

  // Apply filters
  for (const auto& filter : filters) {
    GtkFileFilter *gtk_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(gtk_filter, filter.name.c_str());
    gtk_file_filter_add_pattern(gtk_filter, filter.pattern.c_str());
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), gtk_filter);
  }

  std::filesystem::path result_path;
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    result_path = std::filesystem::path(filename);
    std::cout << "File Selected : " << result_path << std::endl;
    g_free(filename);
  }

  gtk_widget_destroy(dialog);
  return result_path;

#else
  return std::filesystem::path("");
#endif
}
};  // namespace NodeEditor::Utils