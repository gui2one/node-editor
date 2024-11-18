#include "ParamFactory.h"

namespace NED {
ParamFactoryRegistry& NED::ParamFactoryRegistry::instance() {
  {

    static ParamFactoryRegistry registry;
    return registry;
  }
}
std::shared_ptr<NodeParam> ParamFactoryRegistry::create(const std::string& typeName, const char * label) const {
  auto it = factories.find(typeName);
  if (it != factories.end()) {
    auto param = it->second.factory_func();
    param->m_Label = label;
    return param;
  }

  std::cout << "Param NOT found for factory build: " << typeName << std::endl;
  return nullptr;  // Type not found
}
};  // namespace NodeEditor
