#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <Imgui/imgui.h>

namespace QCat
{
	Light::Light(glm::vec3 position, glm::vec3 lightColor, glm::vec3 lightAmbient)
		:lightmodel(position,"",0.4f), rotation(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		info.lightColor = lightColor;
		info.lightAmbient = lightAmbient;
		info.lightPos = position;
	}
	void Light::Bind()
	{
	}
	void Light::Draw(const glm::mat4& cameraTransform, const glm::mat4& proj)
	{
		lightmodel.Draw(cameraTransform, proj,LightInfo());
	}
	void Light::ImGuiRender(const char* name)
	{
		ImGui::Begin(name);

		if (ImGui::DragFloat3("Light Position", glm::value_ptr(info.lightPos), 0.1f))
		{
			lightmodel.SetTranslation(info.lightPos);
		}
		if (ImGui::DragFloat3("Light Rotation", glm::value_ptr(rotation), 0.1f))
		{
			lightmodel.SetRotation(rotation);
		}
		ImGui::ColorEdit3("Light Color", glm::value_ptr(info.lightColor));
		ImGui::ColorEdit3("Light Ambient", glm::value_ptr(info.lightAmbient));
		ImGui::DragFloat("Specular Strength", &info.specularStrength,0.05f,0.1f,1.0f);
		ImGui::DragInt("Shininess", &info.shininess,1.0f,1,64);


		ImGui::End();
	}
}

