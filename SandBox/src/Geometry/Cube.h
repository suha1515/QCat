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
		~Cube() = default;
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);

		void Draw(const Ref<Shader>& shader);

		void SetShader(const Ref<Shader>& shader)
		{
			for (auto& bf : m_VertexArray->GetVertexBuffers())
			{
				bf->SetLayout(BufferLayout::Create(
					{ { ShaderDataType::Float3, "a_Position"},
					  { ShaderDataType::Float3, "a_Normal"   },
					  { ShaderDataType::Float2, "a_TexCoord"},
					}, shader));
			}
		}
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
