#include "ParamFactory.h"

namespace NodeEditor {
ParamFactoryRegistry& NodeEditor::ParamFactoryRegistry::instance() {
    static ParamFactoryRegistry registry;
    return registry;
}
std::shared_ptr<NodeParam> ParamFactoryRegistry::create(const std::string& typeName) const {
  auto it = factories.find(typeName);
  if (it != factories.end()) {
    return it->second.factory_func();
  }
  return nullptr;  // Type not found
}
};  // namespace NodeEditor
