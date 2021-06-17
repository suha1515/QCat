#pragma once
#include <glm/glm.hpp>
#include "QCat/Scene/Entity.h"

namespace QCat
{
	struct NodeData
	{
		Entity entity;
		std::string nodeName;
		std::vector<NodeData> children;
	};
	struct BoneInfo
	{
		// for identifying bone and index bonetransformation in shader
		int id;
		// offsetMatrix transform bone from bone space to local space
		glm::mat4 offsetMatrix;
	};
}
