#include "MeshGenerator.h"

MeshGenerator::MeshGenerator(const char * _title) : ImGuiNode(_title)
{

}

MeshGenerator::~MeshGenerator()
{

}

GridGenerator::GridGenerator(const char * _title): MeshGenerator(_title)
{
}

GridGenerator::~GridGenerator()
{
}
