#include "NodeManager.h"

NodeManager::NodeManager()
{
    
}


NodeManager::~NodeManager()
{

}


void NodeManager::Evaluate()
{
    for(auto node : nodes) {

        node->Update();
    }
}
