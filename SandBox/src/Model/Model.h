#pragma once
#include <QCat.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace QCat
{
	struct BoneStructure;
	class ModelLoader
	{
	public:
		ModelLoader() = default;
		~ModelLoader() = default;
		static Entity  LoadModel(const char* path, const Ref<Scene>& pScene, Entity* parentEntity = nullptr);
	private:
		static Entity LoadModelEntity(const std::string& path, const Ref<Scene>& pScene, Entity* parentEntity = nullptr);
		static Entity ProcessNodeEntity(const std::string& path, aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity);
		static Entity ProcessDynamicNodeEntity(const std::string& path, aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity, std::vector<Entity>& meshEntity, std::vector<std::pair<uint32_t, std::string>>& nodes, BoneStructure& boneStructure);

		static void ReadNodeHierarchy(Entity& node, const Ref<Scene>& pScene, std::vector<std::pair<uint32_t, std::string>>& nodes);


		static Ref<VertexArray> ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene, unsigned index = 0);
		static Ref<VertexArray> ProcessDynamicMesh(aiNode* node, aiMesh* mesh, const aiScene* scene, DynamicMeshComponent& comp, BoneStructure& boneStructure, unsigned index = 0);
		static Material ProcessMaterial(const std::string& path, const aiScene* scene, aiMesh* mesh);



	};
}
