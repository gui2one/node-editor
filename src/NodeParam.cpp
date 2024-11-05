#include "NodeParam.h"

namespace NodeEditor {
void ParamGroup::Display(){
    ImGui::Spacing();
    ImGui::Text("%s", name);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Indent(10.0f);
    ImGui::BeginGroup();

    for(auto item : items){
        item->Display();
    }

    ImGui::EndGroup();
    ImGui::Indent(-10.0f);
}
};
