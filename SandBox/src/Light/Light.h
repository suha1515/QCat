#pragma once

#include "QCat.h"
#include "../Geometry/Sphere.h"

namespace QCat
{
	class Light
	{
	public:
		Light(glm::vec3 position, glm::vec3 lightColor = { 1.0f,1.0f,1.0f });

		glm::vec3 GetColor() const { return lightColor; }

		void Bind();
		void Draw(const glm::mat4& viewProj);
		void ImGuiRender(const char* name);
	private:
		uint32_t uniformBlockID;
		Sphere lightmodel;
		glm::vec3 lightColor = { 1.0f,1.0f,1.0f };

		glm::vec3 translation;
		glm::vec3 rotation;
	};
}