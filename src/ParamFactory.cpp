#include "ParamFactory.h"

namespace NED {
ParamFactoryRegistry& NED::ParamFactoryRegistry::instance() {
  {

    static ParamFactoryRegistry registry;
    return registry;
  }
}
std::shared_ptr<NodeParam> ParamFactoryRegistry::create(const std::string& typeName) const {
  auto it = factories.find(typeName);
  if (it != factories.end()) {
    return it->second.factory_func();
  }

  std::cout << "Param NOT found for factory build: " << typeName << std::endl;
  return nullptr;  // Type not found
}
};  // namespace NodeEditor
