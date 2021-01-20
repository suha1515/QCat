#pragma once

#include "QCat.h"

namespace QCat
{
	class Cube
	{
	public:
		Cube(const glm::vec3& position,const char* texturePath = "");
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);

		void Draw(const glm::mat4& viewProj, const glm::vec3 lightcolor = { 1.0f,1.0f,1.0f });
	public:
		void ImguiRender(const char* name);
	private:
		Ref<Texture2D> m_Texture;
		Ref<VertexArray> m_VertexArray;
		Ref<Shader> shader;

		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;

	};
}
