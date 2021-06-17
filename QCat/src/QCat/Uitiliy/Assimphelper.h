#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
namespace Utils
{
	static inline glm::mat4 ConvertToGlm(aiMatrix4x4 mat)
	{
		return {
			mat.a1, mat.b1, mat.c1, mat.d1,
			mat.a2, mat.b2, mat.c2, mat.d2,
			mat.a3, mat.b3, mat.c3, mat.d3,
			mat.a4, mat.b4, mat.c4, mat.d4
		};
	}
	static inline glm::vec3 ConvertToGlm(const aiVector3D& vector)
	{
		return glm::vec3(vector.x, vector.y, vector.z);
	}
	static inline glm::quat ConvertToGlm(const aiQuaternion& orientation)
	{
		return glm::quat(orientation.w, orientation.x, orientation.y, orientation.z);
	}
}