#include "Scene.h"
#include "Allocator.h"

Entity& Scene::createEntity(std::string name, Mesh* mesh)
{
	Entity ent(name, mesh);

	Entities.push_back(ent);
	return Entities[Entities.size() - 1];
}

std::vector<Entity>& Scene::getEntities()
{
	return Entities;
}

void Scene::setMainCamera(glm::vec3 position, glm::vec3 front, float fov, float aspect, float nearPlane, float farPlane)
{
	this->mainCamera = Mem::Allocate<Camera>(position, front, fov, aspect, nearPlane, farPlane);
}

void Scene::release() {
	for (Entity ent : Entities) {
		ent.release();
	}

	Entities.clear();
}

Camera& Scene::getMainCamera()
{
	return *mainCamera;
}