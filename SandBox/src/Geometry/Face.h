#pragma once

#include "QCat.h"

namespace QCat
{
	class Face
	{
	public:
		Face(const glm::vec3& position, const Ref<Shader>& shader,Material material,uint32_t size=1);
		~Face() = default;
	public:
		void SetScale(const glm::vec3 & scale);
		void SetRotation(const glm::vec3 & rotation);
		void SetTranslation(const glm::vec3& translation);

		void Draw(const Ref<Shader> & shader);

		const Material& GetMaterial() { return material; }
		void SetMaterial(Material& mat) { material = mat; }
		void MaterialBind();
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