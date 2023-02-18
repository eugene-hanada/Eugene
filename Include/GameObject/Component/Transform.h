#pragma once
#include "Component.h"

namespace Eugene
{
    class CommandList;

    class Transform :
        public Component
    {
    public:
        virtual void Update(CommandList& cmdList) = 0;
    private:
    };
}
