#pragma once

#include "QCat.h"
#include "../Geometry/Sphere.h"

namespace QCat
{
	struct LightInfo
	{
		LightInfo()
		{
			lightColor = { 1.0f,1.0f,1.0f };
			lightAmbient = { 0.0f,0.0f,0.0f };
			lightPos = { 0.0f, 0.0f, 0.0f };
			specularStrength = 0.5f;
			shininess = 32;
		}
		glm::vec3 lightColor;
		glm::vec3 lightAmbient;
		glm::vec3 lightPos;
		float specularStrength;
		int shininess;
	};
	class Light
	{
	public:
		Light(glm::vec3 position, glm::vec3 lightColor = { 1.0f,1.0f,1.0f }, glm::vec3 lightAmbient = { 0.1f,0.1f,0.1f });

		LightInfo Getinfo() const { return info; }

		void Bind();
		void Draw(const glm::mat4& cameraTransform, const glm::mat4& proj);
		void ImGuiRender(const char* name);
	private:
		uint32_t uniformBlockID;
		Sphere lightmodel;
		LightInfo info;

		glm::vec3 rotation;
	};
}