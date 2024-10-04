#include "StringGenerator.h"

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

WorldGenerator::WorldGenerator()
{
}

WorldGenerator::~WorldGenerator()
{
}

StringRepeater::StringRepeater()
{
    SetNumAvailableInputs(1);
    Param<uint32_t> param("Count");
    m_Params.push_back(param);
}
StringRepeater::~StringRepeater()
{
}
