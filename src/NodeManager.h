#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "ImGuiNode.h"
#include "EventManager.h"
#include "utils/node_manager_utils.h"

namespace NodeEditor {

struct ConnectionProcedure{
    enum Direction{
        CHILD_TO_PARENT = 0,
        PARENT_TO_CHILD = 1
    };

    Direction direction = CHILD_TO_PARENT;
    bool started = false;
    std::shared_ptr<ImGuiNode> output_node;
    uint32_t output_index;
    std::shared_ptr<ImGuiNode> input_node;
};

struct ViewProperties{
    bool display_grid = true;
    bool nodes_menu_opened = false;
    bool show_mouse_coords = false;
    ImVec2 scrolling;

    float zoom = 1.0f;
    
    bool canvasHovered = false;
    ImVec2 canvasSize;
    ImVec2 canvasPos;    
};

class NodeManager{

public:
    NodeManager();
    ~NodeManager();
    
    /// @brief Computes result for all 'parents' of m_OutputNode.
    /// does nothing if m_OutputNode == nullptr
    void Evaluate();

    inline void SetNodes(std::vector<std::shared_ptr<ImGuiNode>> _nodes) { nodes = _nodes; }
    inline void AddNode(std::shared_ptr<ImGuiNode> _node) { nodes.push_back(_node); }

    inline std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() { return nodes; }
    inline const std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() const { return nodes; }
    std::shared_ptr<ImGuiNode> FindNodeByUUID(std::string uuid);

    void SetNodesMenu(std::function<void()> func);
    void DisplayNodeParams(std::shared_ptr<ImGuiNode> node);
    void DrawNodes();
    void DrawCanvas();

    inline void SetGLFWWindow(GLFWwindow* _window) { m_GLFWWindow = _window; }
    inline GLFWwindow* GetGLFWWindow() { return m_GLFWWindow; }

    void SetOutputNode(std::shared_ptr<ImGuiNode> node);
    std::shared_ptr<ImGuiNode> GetOutputNode() { return m_OutputNode; }


    void ViewFrameAll();

    ImVec2 ToCanvasSpace(ImVec2 pos);
    ImVec2 ToScreenSpace(ImVec2 pos);

    bool IsNodeHovered(std::shared_ptr<ImGuiNode> node);
    bool IsInputConnectorHovered(std::shared_ptr<ImGuiNode> node, uint32_t index);
    
    void ApplyConnectionProcedure();
    void ResetConnectionProcedure();

    void OnMouseMove(const Event& event);
    void OnMouseClick(const Event& event);
    void OnMouseDoubleClick(const Event& event);
    void OnMouseRelease(const Event& event);
    void OnKeyPress(const Event& event);

    inline void SetFonts(ImFont* _regular, ImFont* _bold) { m_RegularFont = _regular; m_BoldFont = _bold; }

public:
    ViewProperties m_ViewProps;
    std::shared_ptr<ImGuiNode> m_CurrentNode = nullptr;
    bool m_OneParamChanged = false;
    std::chrono::time_point<std::chrono::system_clock> m_LastCLickReleaseTime;

    ImFont * m_RegularFont;
    ImFont * m_BoldFont;
private:
    std::vector<std::shared_ptr<ImGuiNode>> nodes;
    GLFWwindow* m_GLFWWindow;
    std::function<void()> m_NodesMenu = [](){};

    ConnectionProcedure m_ConnectionProcedure;
    std::shared_ptr<ImGuiNode> m_OutputNode = nullptr;
}; // end class NodeManager
}; // end namespace NodeEditor
#endif