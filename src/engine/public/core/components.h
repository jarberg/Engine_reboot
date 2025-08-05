#pragma once
#include <string>

#include <core/entity.h>
#include <core/world.h>
#include <core/input.h>

class Entity;

struct Tag
{
	std::string tag;
};

struct DirtyTag :Tag
{
};

struct UuidComponent
{
	std::uint32_t uuid;
	UuidComponent(const std::uint32_t& _uuid) : uuid(_uuid) {}
};

struct PositionComponent
{
	double x, y, z;

};
struct RotationComponent
{
	double x, y, z;
	float matrix[16] = 
	          { 1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1 };
};
struct VelocityComponent
{
	double x, y, z;
};

struct AccelerationComponent
{
	double x, y, z;
};

struct CharacterComponent
{
	
	int callbackID=0;
	Entity owner;

public :
	CharacterComponent(Entity _owner):owner(_owner) {
		InputHandler* input = InputHandler::GetInstance();
		input->inputDispatcher->Subscribe<KeyPressedEvent>(
			[this](std::shared_ptr<KeyPressedEvent> e) {
				this->inputEvent(owner, e);
			}
		);
	}

	void inputEvent(Entity owner, std::shared_ptr<KeyPressedEvent> e);

};

struct StaticMeshComponent {
	int meshID;

	StaticMeshComponent(int _ID):meshID(_ID) {
			
	};
};
