#pragma once

#include "QCat.h"
#include "../Material/Material.h"

namespace QCat
{
	class Face
	{
	public:
		Face(const glm::vec3& position, const Ref<Shader>& shader,uint32_t size=1);
		~Face() = default;
	public:
		void SetScale(const glm::vec3 & scale);
		void SetRotation(const glm::vec3 & rotation);

		void Draw(const Ref<Shader> & shader);
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