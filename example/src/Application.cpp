#include "Application.h"

namespace NED {

Application::Application() {}

bool Application::Init() {
  if (!InitGLFW()) {
    return false;
  }
  GetNodeManager().SetFileExtension("str3000");

  return true;
}

};  // namespace NED