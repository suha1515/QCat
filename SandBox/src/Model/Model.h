#pragma once
#include <QCat.h>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace QCat
{
	class Model
	{
	public:
		Model(const char* path);
		void Draw(const Ref<Shader> & shader);
	private:
		std::vector<Mesh> meshes;
		std::string path;

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Mesh::Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typename);

	};
}