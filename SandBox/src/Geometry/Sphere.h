#pragma once

#include "QCat.h"

namespace QCat
{
	class Sphere
	{
	public:
		Sphere(const glm::vec3& position,const char* texturePath = "", float radius =1.0f,int sectorCount = 36, int stackCount = 18);
	public:
		void SetScale(const glm::vec3& scale);
		void SetRotation(const glm::vec3& rotation);
		void SetTranslation(const glm::vec3 translation) { this->translation = translation; }

		void Draw(const glm::mat4& viewProj, const glm::vec3 lightcolor = { 1.0f,1.0f,1.0f });

		Shader* GetShader() { return shader.get(); }
		void SetShader(const Ref<Shader>& shader)
		{
			if (this->shader)
				this->shader.reset();
			
			this->shader = shader;
		}
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