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

template<typename T>
void Entity::AddScript() {

	// Use proper allocator for this
	T* script = new T();

	if (!dynamic_cast<Script*>(script)) {
		Logger::logError("Passed class is not a subclass of script");
		delete script;
		return;
	}
	script->attachParentEntity(this);
	script->scene = EngineContext::get()->sceneManager;
	script->Start();
	scripts.push_back(script);
}