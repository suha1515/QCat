#pragma once
#include "QCat.h"

namespace QCat
{
	class Cube
	{
	public:
		Cube(const glm::vec3& position);
		~Cube() = default;
		static void Initialize();
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);
		void SetTranslation(const glm::vec3& translation);
		const glm::mat4 GetTransform();

		void Draw();
		Ref<VertexArray>& GetVertexArray() { return m_VertexArray; }
	public:
		void ImguiRender(const char* name);
	private:
		Ref<VertexArray> m_VertexArray;

		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}
