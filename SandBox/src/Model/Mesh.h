#pragma once
#include <QCat.h>
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
		};
		struct Texture
		{
			std::string type;
			Ref<Texture2D> texture;
		};
		Mesh(std::vector<Vertex>& vertices,std::vector<unsigned int>& indices,std::vector<Texture>& textures,const Ref<Shader>& shader);
		void Draw(const Ref<Shader>& shader);

		void Initialize(const Ref<Shader>& shader);
	private:
		Ref<VertexArray> m_VertexArray;
		
		// Mesh Data//
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> m_Textures;
	};
}
