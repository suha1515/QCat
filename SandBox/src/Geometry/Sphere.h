#pragma once

#include "QCat.h"
#include "../Material/Material.h"
namespace QCat
{
	struct LightInfo;
	class Sphere
	{
	public:
		Sphere(const glm::vec3& position,const Ref<Shader>&shader, float radius =1.0f,int sectorCount = 36, int stackCount = 18);
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);
		void SetTranslation(const glm::vec3 translation) { this->translation = translation; }

		void Draw(const Ref<Shader>& shader);

		glm::vec3 GetTranslation() { return translation; }
	public:
		void ImguiRender(const char* name);
	private:
		Ref<Texture2D> m_Texture;
		Ref<VertexArray> m_VertexArray;

		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;

		Material material;
	};
	
}