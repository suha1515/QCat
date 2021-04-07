#pragma once
#include <QCat.h>
#include "../Material/Material.h"
namespace QCat
{
	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texcoords;
			glm::vec3 tangent;
			glm::vec3 bitangent;
		};
		struct Texture
		{
			std::string type;
			Ref<Texture2D> texture;
		};
		Mesh(const glm::mat4& transform,std::vector<Vertex>& vertices,std::vector<unsigned int>& indices,const Ref<Shader>& shader);
		void Draw();
		void Initialize(const Ref<Shader>& shader, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

		const glm::mat4& GetTransform();
	private:
		Ref<VertexArray> m_VertexArray;
		glm::mat4 transform = glm::mat4(1.0f);
	};
}