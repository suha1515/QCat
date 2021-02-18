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
		Mesh(std::vector<Vertex>& vertices,std::vector<unsigned int>& indices,Material material,const Ref<Shader>& shader);
		void Draw(const Ref<Shader>& shader, const glm::mat4& transform);

		void Initialize(const Ref<Shader>& shader, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	private:
		Ref<VertexArray> m_VertexArray;
		Material material;
	};
}