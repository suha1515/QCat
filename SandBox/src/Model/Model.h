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
		void Draw();

		void SetScale(const glm::vec3& scale) { this->scale = scale; };
		void SetRotation(const glm::vec3& rotation) { this->rotation = rotation; }
		void SetTranslation(const glm::vec3& translation) {this->translation = translation;}
		const glm::mat4 GetTransform();
		std::vector<Mesh>& GetMeshes() { return meshes; };

		static Ref<Model> Create(const char* path);
	private:
		std::vector<Mesh> meshes;
		std::string path;

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene);
		std::vector<Mesh::Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typename);

		glm::vec3 translation = { 0.0f,0.0f,0.0f };
		glm::vec3 rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 scale = { 1.0f,1.0f,1.0f };


	};
}