#pragma once
#include "GameObject.h"

namespace Eugene
{
    class Scene :
        public GameObject
    {
    public:
        using SceneUPtr = std::unique_ptr<Scene>;
        SceneUPtr&& Update(SceneUPtr&& scene);
    private:
    };
}
