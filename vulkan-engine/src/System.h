#pragma once

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include "Allocator.h"

class ISystem {
public:
	virtual void startup() = 0;
	virtual void update(float deltaTime) = 0;
	virtual void shutdown() = 0;

	ISystem() = default;
	std::string name;
};

#endif 
