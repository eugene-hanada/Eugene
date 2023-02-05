#include "../../Include/GameObject/Scene.h"

Eugene::Scene::SceneUPtr&& Eugene::Scene::Update(SceneUPtr&& scene)
{
	return std::move(scene);
}
