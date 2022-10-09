#pragma once

#ifndef SCRIPTING_SYSTEM
#define SCRIPTING_SYSTEM

#include "Core/System.h"
#include "Scene.h"

class ScriptingSystem : public ISystem {
public:
	virtual void startup() override;
	virtual void update(float deltaTime) override;
	virtual void shutdown() override;
};

#endif // !SCRIPTING_SYSTEM
