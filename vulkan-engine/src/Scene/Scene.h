#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "Scene/Entity.h"
#include "Camera.h"
#include <vector>

class Scene {
protected:
	std::vector<Entity> Entities;	
	Camera* mainCamera;
public:
	Entity& createEntity(std::string name, Mesh* mesh = nullptr);
	std::vector<Entity>& getEntities();
	void setMainCamera(glm::vec3 position, glm::vec3 front, float fov, float aspect, float near, float far);

	Camera& getMainCamera();
	void release();
};

#endif // !SCENE_H
