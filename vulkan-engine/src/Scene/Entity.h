#pragma once

#ifndef  ENTITY_H
#define ENTITY_H

#include "Renderer/Vertex.h"
#include <string>
#include "Transform.h"
#include "Script.h"

class Entity {
protected:
	Mesh* mesh;
	std::string name;

public:
	Transform transform;
	std::vector<Script*> scripts;

	Entity(std::string name, Mesh* mesh);
	Entity(std::string name);

	Mesh& getMesh() { return *mesh; }
	std::string getName() { return name; }

	template<typename T>
	void AddScript();
};

#endif // ! ENTITY_H
