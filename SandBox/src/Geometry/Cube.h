#pragma once

#include "QCat.h"
#include "../Material/Material.h"
namespace QCat
{
	struct LightInfo;
	class Cube
	{
	public:
		Cube(const glm::vec3& position);
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);

		void Draw(const glm::mat4& cameraTransform, const glm::mat4& proj, LightInfo info);
	public:
		void ImguiRender(const char* name);
	private:
		Ref<VertexArray> m_VertexArray;
		Ref<Shader> shader;

		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;

		Material material;

	};
}
