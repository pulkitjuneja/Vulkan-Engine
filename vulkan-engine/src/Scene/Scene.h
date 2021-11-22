#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "Scene/Entity.h"
#include <vector>

class Scene {
protected:
	std::vector<Entity> Entities;	
public:
	Entity& createEntity(std::string name, Mesh* mesh = nullptr);
	std::vector<Entity>& getEntities();

	void release();
};

#endif // !SCENE_H
