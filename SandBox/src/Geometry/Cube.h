#pragma once

#include "QCat.h"
#include "../Material/Material.h"
namespace QCat
{
	class Cube
	{
	public:
		Cube(const glm::vec3& position,const Ref<Shader>& shader);
		~Cube() = default;
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);
		void SetTranslation(const glm::vec3& translation);

		void Draw(const Ref<Shader>& shader);

		void SetMaterial(Material& mat) { material = mat; }
	public:
		void ImguiRender(const char* name);
	private:
		Ref<VertexArray> m_VertexArray;

		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;

		Material material;

	};
}
