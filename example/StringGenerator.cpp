#include "StringGenerator.h"
namespace NodeEditor {

StringGenerator::StringGenerator()
{
    SetNumAvailableInputs(0);
}

StringGenerator::~StringGenerator()
{

}

StringModifier::StringModifier()
{

}

StringModifier::~StringModifier()
{

}

HelloGenerator::HelloGenerator(): StringGenerator()
{
    
}

HelloGenerator::~HelloGenerator()
{
}

StringConcatenator::StringConcatenator()
{
    SetNumAvailableInputs(2);
}

StringConcatenator::~StringConcatenator()
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