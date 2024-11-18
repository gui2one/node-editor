#ifndef NODEMANAGER_H
#define NODEMANAGER_H
#pragma once

#include <stb_image.h>

#include "EventManager.h"
#include "ImGuiNode.h"
#include "ParamFactory.h"
#include "utils/node_manager_utils.h"

namespace NED {

struct WindowData {
  int width = 1280;
  int height = 720;

  int mouseX = 0;
  int mouseY = 0;
  const char* title = "Stringinator 3000";

  std::filesystem::path current_path;
};

struct ConnectionProcedure {
  enum Direction { CHILD_TO_PARENT = 0, PARENT_TO_CHILD = 1 };

  Direction direction = CHILD_TO_PARENT;
  bool started = false;
  std::shared_ptr<AbstractNode> output_node;
  uint32_t output_index;
  std::shared_ptr<AbstractNode> input_node;
  bool is_mutli_input = false;
};

struct Rect{
  float x;
  float y;
  float width;
  float height;
};

struct ViewProperties {
  bool display_grid = true;
  bool nodes_menu_opened = false;
  bool show_mouse_coords = false;
  ImVec2 scrolling;

  float zoom = 1.0f;

  bool canvasHovered = false;
  
  bool rectangleSelectionStarted = false;
  ImVec2 rectangleSelectionStartPoint;
  ImVec2 rectangleSelectionEndPoint;

  ImVec2 canvasSize;
  ImVec2 canvasPos;
};

struct IconResource {
  const char* name;
  std::filesystem::path path;
};

class NodeManager {
 public:
  NodeManager();
  ~NodeManager();

  void InitGLFWEvents();

  void InitIcons();
  void AddIcon(const char* name, std::filesystem::path path);

  /// @brief Computes result for all 'parents' of m_OutputNode.
  /// does nothing if m_OutputNode == nullptr
  void Evaluate();

  inline void SetNodes(std::vector<std::shared_ptr<AbstractNode>> _nodes) { m_NodeNetwork.nodes = _nodes; }
  inline void AddNode(std::shared_ptr<AbstractNode> _node) { m_NodeNetwork.AddNode(_node); }

  inline std::vector<std::shared_ptr<AbstractNode>>& GetNodes() { return m_CurrentNetwork->nodes; }
  inline const std::vector<std::shared_ptr<AbstractNode>>& GetNodes() const { return m_CurrentNetwork->nodes; }
  std::shared_ptr<AbstractNode> FindNodeByUUID(std::string uuid);

  void DisplayNodeParams(std::shared_ptr<AbstractNode> node);
  void DrawNodes();
  void DrawCanvas();

  inline void SetGLFWWindow(GLFWwindow* _window) { m_GLFWWindow = _window; }
  inline GLFWwindow* GetGLFWWindow() { return m_GLFWWindow; }

  void SetOutputNode(std::shared_ptr<AbstractNode> node);
  inline void UnsetOutputNode() {
    if (m_CurrentNetwork != nullptr) m_CurrentNetwork->outuput_node = nullptr;
  };
  std::shared_ptr<AbstractNode> GetOutputNode() {
    if (m_CurrentNetwork != nullptr) {
      return m_CurrentNetwork->outuput_node;
    } else {
      return nullptr;
    }
  }
  NodeNetwork* GetCurrentNetwork() { return m_CurrentNetwork; }
  NodeNetwork& GetRootNetwork() { return m_NodeNetwork; };
  void GotoRootNetwork() {
    m_CurrentNetworkOwner = nullptr;
    m_CurrentNetwork = &m_NodeNetwork;
  };
  void ViewFrameAll();

  ImVec2 ToCanvasSpace(ImVec2 pos);
  ImVec2 ToScreenSpace(ImVec2 pos);

  bool IsNodeHovered(std::shared_ptr<AbstractNode> node);
  bool IsInputConnectorHovered(std::shared_ptr<AbstractNode> node, uint32_t index);
  bool IsNodeMultiInputConnectorHovered(std::shared_ptr<AbstractNode> node);

  void ApplyConnectionProcedure();
  void ResetConnectionProcedure();

  inline void SetFileExtension(const char* _ext) { m_FileExtension = _ext; }
  void SaveAll();
  void LoadAll();
  void LoadFromFile(std::filesystem::path path);

  void OnMouseMove(const Event& event);
  void OnMouseClick(const Event& event);

  void OnMouseRelease(const Event& event);
  void OnKeyPress(const Event& event);

  void OnMouseDoubleClick(const Event& event) {
    if (m_CurrentNode == nullptr) {
      return;
    }
    if (IsNodeHovered(m_CurrentNode)) {
      if (m_CurrentNode->IsSubnet()) {
        m_CurrentNetworkOwner = m_CurrentNode.get();
        m_CurrentNetwork = &m_CurrentNode->node_network;
      }
    }
  }

  inline void SetFonts(ImFont* _regular, ImFont* _bold) {
    m_RegularFont = _regular;
    m_BoldFont = _bold;
  }

  void CreateAllNodes();
 private:
  void SetNodesMenu(std::function<void()> func);
  void BuildNodeMenuFromRegistry();

 public:
  WindowData m_WindowData;
  ViewProperties m_ViewProps;
  AbstractNode* m_CurrentNetworkOwner = nullptr;
  std::shared_ptr<AbstractNode> m_CurrentNode = nullptr;
  std::filesystem::path m_SavePath = "";
  bool m_OneParamChanged = false;
  std::chrono::time_point<std::chrono::system_clock> m_LastCLickReleaseTime;

  ImFont* m_RegularFont;
  ImFont* m_BoldFont;

  std::vector<IconResource> m_NodeIconsResources = {{"arrow", "resources/icons/arrow_1.png"},
                                                    {"empty_set", "resources/icons/empty_set.png"},
                                                    {"three_dots", "resources/icons/three_dots.png"}};

  std::map<const char*, GLuint> m_Icons;

 private:
  NodeNetwork m_NodeNetwork;
  NodeNetwork* m_CurrentNetwork;

  GLFWwindow* m_GLFWWindow;
  std::function<void()> m_NodesMenu = []() {};

  ConnectionProcedure m_ConnectionProcedure;
  const char* m_FileExtension = "ney";
};  // end class NodeManager

};  // end namespace NodeEditor

#endif