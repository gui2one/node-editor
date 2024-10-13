#include "node_manager_utils.h"

namespace NodeEditor::Utils {
    ImVec2 to_canvas_space(ImVec2 pos, ImVec2 origin, float zoom) {
        return ImVec2(pos) - origin * zoom;
    }
}