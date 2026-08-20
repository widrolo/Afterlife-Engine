#include "BehaviourTree.h"


using namespace WEngine::Behaviour;

BehaviourTree::BehaviourTree()
{
    //m_blackboard.AddVariable("self", owner);
}

void BehaviourTree::TickBrain()
{
    m_root->Execute();
}
