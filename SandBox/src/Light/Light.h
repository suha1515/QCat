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
			ambient = { 0.2f,0.2f,0.2f };
			specular = { 1.0f,1.0f,1.0f };

			lightPosition = { 0.0f, 0.0f, 0.0f };
			lightDirection = { 1.0f,0.0f,0.0f };

			constant = 1.0f;
			linear = 0.09f;
			quadratic = 0.032f;
			cutoff = glm::cos(glm::radians(12.5f));
			outerCutOff = glm::cos(glm::radians(17.5f));
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
		void SetDiffuse(const glm::vec3& diffuse) { info.diffuse = diffuse; }
		void SetAmbient(const glm::vec3& ambient) { info.ambient = ambient; }
		void SetSpecular(const glm::vec3& specular) { info.specular = specular; }

		void SetLightinfo(LightInfo& info) { this->info = info; }
		void ImGuiRender(const char* name);
		LightInfo info;
	private:
		
		
		glm::vec3 rotation;
	};
}