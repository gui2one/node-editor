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
    bool is_mutli_input = false;
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

    inline void SetNodes(NODE_COLLECTION _nodes) { m_NodeNetwork.nodes = _nodes; }
    inline void AddNode(std::shared_ptr<ImGuiNode> _node) { m_NodeNetwork.nodes.push_back(_node); }

    inline NODE_COLLECTION& GetNodes() { return m_CurrentNetwork->nodes; }
    inline const NODE_COLLECTION& GetNodes() const { return m_CurrentNetwork->nodes; }
    std::shared_ptr<ImGuiNode> FindNodeByUUID(std::string uuid);

    void DisplayNodeParams(std::shared_ptr<ImGuiNode> node);
    void DrawNodes();
    void DrawCanvas();

    inline void SetGLFWWindow(GLFWwindow* _window) { m_GLFWWindow = _window; }
    inline GLFWwindow* GetGLFWWindow() { return m_GLFWWindow; }

    void SetOutputNode(std::shared_ptr<ImGuiNode> node);
    inline void UnsetOutputNode() { if(m_CurrentNetwork != nullptr) m_CurrentNetwork->outuput_node = nullptr; };
    std::shared_ptr<ImGuiNode> GetOutputNode() { 
        if(m_CurrentNetwork != nullptr) {
            return m_CurrentNetwork->outuput_node;
        }else{
            return nullptr;
        } 
        
    }


    void ViewFrameAll();

    ImVec2 ToCanvasSpace(ImVec2 pos);
    ImVec2 ToScreenSpace(ImVec2 pos);

    bool IsNodeHovered(std::shared_ptr<ImGuiNode> node);
    bool IsInputConnectorHovered(std::shared_ptr<ImGuiNode> node, uint32_t index);
    bool IsNodeMultiInputConnectorHovered(std::shared_ptr<ImGuiNode> node);
    
    void ApplyConnectionProcedure();
    void ResetConnectionProcedure();

    void OnMouseMove(const Event& event);
    void OnMouseClick(const Event& event);
    void OnMouseDoubleClick(const Event& event);
    void OnMouseRelease(const Event& event);
    void OnKeyPress(const Event& event);

    inline void SetFonts(ImFont* _regular, ImFont* _bold) { m_RegularFont = _regular; m_BoldFont = _bold; }

private:
    void SetNodesMenu(std::function<void()> func);
    void BuildNodeMenuFromRegistry();
public:
    ViewProperties m_ViewProps;
    std::shared_ptr<ImGuiNode> m_CurrentNode = nullptr;
    bool m_OneParamChanged = false;
    std::chrono::time_point<std::chrono::system_clock> m_LastCLickReleaseTime;

    ImFont * m_RegularFont;
    ImFont * m_BoldFont;
private:
    NodeNetwork m_NodeNetwork;
    NodeNetwork* m_CurrentNetwork;
    GLFWwindow* m_GLFWWindow;
    std::function<void()> m_NodesMenu = [](){};

    ConnectionProcedure m_ConnectionProcedure;

}; // end class NodeManager
}; // end namespace NodeEditor
#endif