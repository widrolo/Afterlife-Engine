#pragma once
#include "BehaviourNode.h"
#include "Engine/Types/Blackboard.h"


namespace WEngine::Behaviour
{
    class BehaviourTree
    {
    public:
        BehaviourTree();
        ~BehaviourTree() = default;

    public:
        void TickBrain();

    private:
        Blackboard m_blackboard;
        BehaviourNode* m_root;
    };
}
