#include "Scene.h"

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

void Scene::release() {
	for (Entity ent : Entities) {
		ent.release();
	}

	Entities.clear();
}