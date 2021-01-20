#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <Imgui/imgui.h>

namespace QCat
{
	Light::Light(glm::vec3 position, glm::vec3 lightColor)
		:translation(position),lightColor(lightColor),lightmodel(position,"",0.4f), rotation(glm::vec3(0.0f, 0.0f, 0.0f))
	{

	}
	void Light::Bind()
	{
	}
	void Light::Draw(const glm::mat4& viewProj)
	{
		lightmodel.Draw(viewProj);
	}
	void Light::ImGuiRender(const char* name)
	{
		ImGui::Begin(name);

		if (ImGui::DragFloat3("Light Position", glm::value_ptr(translation), 0.1f))
		{
			lightmodel.SetTranslation(translation);
		}
		if (ImGui::DragFloat3("Light Rotation", glm::value_ptr(rotation), 0.1f))
		{
			lightmodel.SetRotation(rotation);
		}
		ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));

		ImGui::End();
	}
}

