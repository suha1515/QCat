#pragma once

#include "QCat.h"
#include "../Geometry/Sphere.h"

namespace QCat
{
	enum class LightType
	{
		Directional =0,Point,Spot
	};
	struct LightInfo
	{
		LightInfo()
		{
			diffuse = { 1.0f,1.0f,1.0f };
			ambient = { 0.0f,0.0f,0.0f };
			specular = { 1.0f,1.0f,1.0f };

			lightPosition = { 0.0f, 0.0f, 0.0f };
			lightDirection = { 1.0f,0.0f,0.0f };

			constant = 0.0;
			linear = 0.0f;
			quadratic = 0.0f;
			cutoff = 0.0f;
			outerCutOff = 0.0f;
		}
		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 lightPosition;
		glm::vec3 lightDirection;

		float constant=1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
		float cutoff = glm::cos(glm::radians(12.5f));
		float outerCutOff = glm::cos(glm::radians(17.5f));
		LightType type = LightType::Directional;
	};
	class Light
	{
	public:
		Light(glm::vec3 position = { 0.0f,0.0f,0.0f }, glm::vec3 lightColor = { 1.0f,1.0f,1.0f }, glm::vec3 lightAmbient = { 0.1f,0.1f,0.1f });

		LightInfo Getinfo() const { return info; }
		void SetDirection(const glm::vec3& dir) { info.lightDirection = dir; }
		void SetPosition(const glm::vec3& pos) { info.lightPosition = pos; }
		void Bind();
		void Draw(const glm::mat4& cameraTransform, const glm::mat4& proj);
		void ImGuiRender(const char* name);
	private:
		uint32_t uniformBlockID;
		LightInfo info;
		
		glm::vec3 rotation;
	};
}