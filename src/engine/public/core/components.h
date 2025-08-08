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
struct CameraComponent
{
	float FOV = 45.0f; // Field of View in degrees
	float fovYRadians = FOV * 3.141592;
	float aspect = 1280/720;
	float nearPlane = 0.1;
	float farPlane = 100;

	double x, y, z;

	std::array<float, 16> perspectiveMatrix =
		{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	std::array<float, 16> cameraMatrix =
		{ 1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, -5, 1 };
	CameraComponent();
	CameraComponent(float FOV,float _aspect, float _nearPlane, float _farPlane) 
		: FOV(FOV), aspect(_aspect), nearPlane(_nearPlane), farPlane(_farPlane) {
		fovYRadians = FOV * 3.141592 / 180.0f;
		createPerspectiveMatrix(fovYRadians, aspect, nearPlane, farPlane, perspectiveMatrix);
	}

	void createPerspectiveMatrix(float fovYRadians, float aspect, float nearPlane, float farPlane, std::array<float, 16>& out) {
		float f = 1.0f / std::tan(fovYRadians / 2.0f);

		out[0] = f / aspect;
		out[1] = 0.0f;
		out[2] = 0.0f;
		out[3] = 0.0f;

		out[4] = 0.0f;
		out[5] = f;
		out[6] = 0.0f;
		out[7] = 0.0f;

		out[8] = 0.0f;
		out[9] = 0.0f;
		out[10] = (farPlane + nearPlane) / (nearPlane - farPlane); // OpenGL-style
		out[11] = -1.0f;

		out[12] = 0.0f;
		out[13] = 0.0f;
		out[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane); // OpenGL-style
		out[15] = 0.0f;
	}
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
