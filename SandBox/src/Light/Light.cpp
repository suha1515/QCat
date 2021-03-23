#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <Imgui/imgui.h>

namespace QCat
{
	Light::Light(glm::vec3 position, glm::vec3 lightColor, glm::vec3 lightAmbient)
	{
		info.diffuse = lightColor;
		info.ambient = lightAmbient;
		info.lightPosition = position;
	}
	void Light::ImGuiRender(const char* name)
	{
		ImGui::Begin(name);

		ImGui::ColorEdit3("Light Color", glm::value_ptr(info.diffuse));
		ImGui::ColorEdit3("Light Ambient", glm::value_ptr(info.ambient));
		ImGui::ColorEdit3("Specular Color", glm::value_ptr(info.specular),0.1f);

		ImGui::DragFloat3("Light Direction", glm::value_ptr(info.lightDirection), 0.1f);
		ImGui::DragFloat3("Light Position", glm::value_ptr(info.lightPosition), 0.1f);

		ImGui::DragFloat("spotlight cutoff", &info.cutoff, 0.01f);
		ImGui::DragFloat("point light constant",  &info.constant,0.1f);
		ImGui::DragFloat("point light linear",    &info.linear, 0.1f);
		ImGui::DragFloat("point light quadratic", &info.quadratic, 0.1f);


		ImGui::End();
	}
}

