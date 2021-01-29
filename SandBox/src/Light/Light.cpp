#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <Imgui/imgui.h>

namespace QCat
{
	Light::Light(glm::vec3 position, glm::vec3 lightColor, glm::vec3 lightAmbient)
		:lightmodel(position,0.1f), rotation(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		info.diffuse = lightColor;
		info.ambient = lightAmbient;
		info.lightPosition = position;
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

		if (ImGui::DragFloat3("Light Position", glm::value_ptr(info.lightPosition), 0.1f))
		{
			lightmodel.SetTranslation(info.lightPosition);
		}
		if (ImGui::DragFloat3("Light Rotation", glm::value_ptr(rotation), 0.1f))
		{
			lightmodel.SetRotation(rotation);
		}
		ImGui::ColorEdit3("Light Color", glm::value_ptr(info.diffuse));
		ImGui::ColorEdit3("Light Ambient", glm::value_ptr(info.ambient));
		ImGui::ColorEdit3("Specular Color", glm::value_ptr(info.specular),0.1f);

		ImGui::DragFloat3("Light Direction", glm::value_ptr(info.lightDirection), 0.1f);

		ImGui::DragFloat("spotlight cutoff", &info.cutoff, 0.01f);
		ImGui::DragFloat("point light constant",  &info.constant);
		ImGui::DragFloat("point light linear",    &info.linear);
		ImGui::DragFloat("point light quadratic", &info.quadratic);


		ImGui::End();
	}
}

