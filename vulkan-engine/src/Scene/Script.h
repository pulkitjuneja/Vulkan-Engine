#pragma once

#ifndef SCRIPT_H
#define SCRIPT_H

class Entity;
class SceneManager;

class Script
{
	friend Entity;
protected:

	Entity* parent;
	SceneManager* scene;

public:
	void attachParentEntity(Entity* entity) { parent = entity; }
	virtual void Start() {}
	virtual void Update(float deltaTime) {}

	virtual void onCollisionBegin(Entity& other) {};
	virtual void onCollisionEnd(Entity& other) {};
};

#endif // !SCRIPT_H