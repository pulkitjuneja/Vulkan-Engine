#include "Scene.h"
#include "Core/Allocator.h"

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

DirectionalLight& Scene::createDirectionalLight(glm::vec4 direction, glm::vec4 diffuse, float intensity)
{
	DirectionalLight light;
	light.diffuse = diffuse;
	light.direction = direction;
	light.intensity = intensity;
	this->directionalLight = light;
	return light;
}

void Scene::release() {

	Entities.clear();
}

Camera& Scene::getMainCamera()
{
	return *mainCamera;
}

DirectionalLight& Scene::getDirectionalLight()
{
	return directionalLight;
}
