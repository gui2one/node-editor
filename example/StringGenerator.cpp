#include "StringGenerator.h"

StringGenerator::StringGenerator()
{

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