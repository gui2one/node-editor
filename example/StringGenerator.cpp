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

