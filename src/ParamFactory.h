#ifndef NODE_EDITOR_PARAMFACTORY_H
#define NODE_EDITOR_PARAMFACTORY_H
#pragma once

#include <unordered_map>

#include "NodeParam.h"

namespace NED {
struct ParamFactoryRegistryItem {
  std::string type_name;
  std::function<std::shared_ptr<NodeParam>()> factory_func;
};
class ParamFactoryRegistry {
 public:
  static ParamFactoryRegistry& instance();
  inline void registerType(const std::string& typeName, ParamFactoryRegistryItem item) { factories[typeName] = item; }
  std::shared_ptr<NodeParam> create(const std::string& typeName) const;

  inline std::unordered_map<std::string, ParamFactoryRegistryItem> getFactories() const { return factories; }

 private:
  std::unordered_map<std::string, ParamFactoryRegistryItem> factories;
};
}  // namespace NodeEditor

#define REGISTER_PARAM_TYPE(Type)                            \
  std::cout << "Registering " << #Type << std::endl;         \
  NED::ParamFactoryRegistry::instance().registerType( \
      #Type, {#Type, []() -> std::shared_ptr<Type> { return std::make_shared<Type>(); }})

#endif