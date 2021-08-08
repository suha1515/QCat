#pragma once
#include <QCat.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace QCat
{
	class Model
	{

	};
	class AnimatedModel
	{

	};

	class ModelLoader
	{
	public:
		ModelLoader() = default;
		~ModelLoader() = default;
		static Entity  LoadModel(const char* path, const Ref<Scene>& pScene);
	private:
		static Entity LoadModelEntity(const std::string& path, const Ref<Scene>& pScene);
		static Entity ProcessNodeEntity(const std::string& path,aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity);
		static Ref<VertexArray> ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene);
		static Material ProcessMaterial(const std::string& path,const aiScene* scene , aiMesh* mesh);
	};
}
