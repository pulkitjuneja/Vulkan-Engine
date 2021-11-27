#include "Entity.h"
#include "Entity.h"

Entity::Entity(std::string name, Mesh* mesh) {
	this->name = name;
	this->mesh = mesh;
}

Entity::Entity(std::string name)
{
	this->name = name;
}