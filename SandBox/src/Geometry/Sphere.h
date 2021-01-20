#pragma once

#include "QCat.h"

namespace QCat
{
	class Sphere
	{
	public:
		Sphere(Scene* activeScene, const char* name, const glm::vec3& position,const char* texturePath = "", float radius =1.0f,int sectorCount = 36, int stackCount = 18);
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);

		void Draw(Entity& camera);
	public:
		void ImguiRender(const char* name);
	private:
		Ref<Texture2D> m_Texture;
		Ref<VertexArray> m_VertexArray;
		Ref<Shader> shader;

		Entity entity;
	};
	
}