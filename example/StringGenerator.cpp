#include "StringGenerator.h"
namespace NodeEditor {



HelloGenerator::HelloGenerator(): StringGenerator()
{
    
}

HelloGenerator::~HelloGenerator()
{
}


WorldGenerator::WorldGenerator():StringGenerator()
{
}

WorldGenerator::~WorldGenerator()
{
}

StringRepeater::StringRepeater():StringModifier()
{
    SetNumAvailableInputs(1);
    auto param = std::make_shared<Param<uint32_t>>("Count", 10);
    m_Params.push_back(param);
}

StringRepeater::~StringRepeater()
{
}

};