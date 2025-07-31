#pragma once
#include <string>


namespace engine {

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

	struct VelocityComponent
	{
		double x, y, z;
	};

	struct AccelerationComponent
	{
		double x, y, z;
	};

	struct StaticMeshComponent {
		int meshID;

		StaticMeshComponent(int _ID):meshID(_ID) {
			
		};
	};
}