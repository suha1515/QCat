#include "ModelLoader.h"

#include <glm/gtx/matrix_decompose.hpp>

namespace QCat
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};
	struct AnimatedVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		int boneIDs[4];
		float weights[4];
	};
	struct BoneStructure
	{
		std::unordered_map<std::string, BoneInfo> m_OffsetMatMap;
		uint32_t boneCount = 0;
	};
	namespace Utils
	{
		glm::mat4 ConvertToGlm(aiMatrix4x4 mat)
		{
			return {
				mat.a1, mat.b1, mat.c1, mat.d1,
				mat.a2, mat.b2, mat.c2, mat.d2,
				mat.a3, mat.b3, mat.c3, mat.d3,
				mat.a4, mat.b4, mat.c4, mat.d4
			};
		}
	}
	Entity ModelLoader::LoadModel(const char* path, const Ref<Scene>& pScene, Entity* parentEntity)
	{
		auto& list = ModelLibrary::GetModelList();
		auto iter = std::find_if(list.begin(), list.end(), [path](const std::string& modelpath) {
			if (path == modelpath)
				return true;
			else
				return false;
			});
		if (iter == list.end())
			list.push_back(path);

		return LoadModelEntity(path, pScene, parentEntity);
	}
	Entity ModelLoader::LoadModelEntity(const std::string& path, const Ref<Scene>& pScene, Entity* parentEntity)
	{
		Assimp::Importer importer;

		unsigned int flag;
		flag = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_MakeLeftHanded |
			aiProcess_FlipWindingOrder| aiProcess_LimitBoneWeights;

		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
			flag |= aiProcess_FlipUVs;

		const aiScene* scene = importer.ReadFile(path,
			flag);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "Error:Assimp:";
			QCAT_ASSERT(false, error + importer.GetErrorString());
		}

		Entity rootNode;
		std::vector<std::pair<uint32_t,std::string>> nodes;
		std::vector<Entity> meshEntity;
		BoneStructure bonestructure;
		bool HasAnim = scene->HasAnimations();
		if (HasAnim)
		{
			rootNode = ProcessDynamicNodeEntity(path, scene->mRootNode, scene, pScene, parentEntity, meshEntity, nodes, bonestructure);
			for (int i = 0; i < meshEntity.size(); ++i)
			{
				meshEntity[i].GetComponent<DynamicMeshComponent>().Initialize(nodes);
			}
			rootNode.AddComponent<AnimatorComponent>().animator.Initialize(scene,nodes, bonestructure.m_OffsetMatMap, bonestructure.boneCount);
		}
		else
			rootNode = ProcessNodeEntity(path, scene->mRootNode, scene, pScene,parentEntity);
		
		
		return rootNode;
	}
	void ModelLoader::ReadNodeHierarchy(Entity& node, const Ref<Scene>& pScene, std::vector<std::pair<uint32_t, std::string>>& nodes)
	{
		uint32_t childid = node.GetComponent<RelationShipComponent>().firstChild;
		if (childid != 0xFFFFFFFF)
		{
			Entity childEntity = pScene->GetEntityById(childid);
			ReadNodeHierarchy(childEntity, pScene, nodes);
			childid = childEntity.GetComponent<RelationShipComponent>().nextSibling;
		}
	}
	Entity ModelLoader::ProcessNodeEntity(const std::string& path, aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity)
	{
		// if node has meshes, Node has a meshIndex and Scene has a real mesh so
		// if node want to acess its own meshes , pass the nodes meshIndex to scene->mMeshes
		std::string nodeName = node->mName.C_Str();
		Entity entity = pScene->CreateEntity(nodeName);
		entity.SetParent(parentEntity);

		glm::mat4 transform = Utils::ConvertToGlm(node->mTransformation);
		entity.GetComponent<TransformComponent>().SetTransform(transform);
		if (node->mNumMeshes != 0)
		{
			entity.AddComponent<MeshComponent>();
			entity.AddComponent<MaterialComponent>();		
		}
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			if (i < 1)
			{
				entity.GetComponent<MeshComponent>().vertexArray = ProcessMesh(node, mesh, scene);
				entity.GetComponent<MaterialComponent>().material = ProcessMaterial(path, scene, mesh);
			}	
			else
			{
				Entity sub = pScene->CreateEntity(mesh->mName.data);
				sub.SetParent(&entity);
				sub.AddComponent<MeshComponent>().vertexArray = ProcessMesh(node, mesh, scene, i);
				sub.AddComponent<MaterialComponent>().material = ProcessMaterial(path, scene, mesh);
			}
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNodeEntity(path,node->mChildren[i], scene, pScene,&entity);
		}
		return entity;
	}
	Entity ModelLoader::ProcessDynamicNodeEntity(const std::string& path, aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity, std::vector<Entity>& meshEntity, std::vector<std::pair<uint32_t, std::string>>& nodes, BoneStructure& boneStructure)
	{
		std::string nodeName = node->mName.C_Str();
		Entity entity = pScene->CreateEntity(nodeName);
		entity.SetParent(parentEntity);
		nodes.push_back({ entity.GetUID(),nodeName });

		glm::mat4 transform = Utils::ConvertToGlm(node->mTransformation);
		entity.GetComponent<TransformComponent>().SetTransform(transform);
		if (node->mNumMeshes != 0)
		{
			entity.AddComponent<MaterialComponent>();
			entity.AddComponent<DynamicMeshComponent>();
		}
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			if (i < 1)
			{
				auto& comp = entity.GetComponent<DynamicMeshComponent>();
				comp.vertexArray = ProcessDynamicMesh(node, mesh, scene, comp, boneStructure);
				comp.modelPath = path;
				entity.GetComponent<MaterialComponent>().material = ProcessMaterial(path, scene, mesh);
				meshEntity.push_back(entity);
			}
			else
			{
				Entity sub = pScene->CreateEntity(mesh->mName.data);
				sub.SetParent(&entity);
				nodes.push_back({ sub.GetUID(),mesh->mName.data });
				auto& comp = sub.AddComponent<DynamicMeshComponent>();
				comp.vertexArray = ProcessDynamicMesh(node, mesh, scene, comp, boneStructure);
				comp.modelPath = path;
				sub.AddComponent<MaterialComponent>().material = ProcessMaterial(path, scene, mesh);
				meshEntity.push_back(sub);
			}
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessDynamicNodeEntity(path, node->mChildren[i], scene, pScene, &entity,meshEntity,nodes,boneStructure);
		}
		return entity;
	}

	Ref<VertexArray> ModelLoader::ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene, unsigned index)
	{
		// Check is there mesh in library
		std::string meshName = mesh->mName.C_Str();
		if (meshName == "")
		{
			meshName = node->mName.C_Str();
			meshName += "_Mesh";
		}
		if (index != 0)
			meshName = meshName + "_" + std::to_string(index);
		Ref<VertexArray> vertarray = MeshLibrary::Load(meshName);
		if (vertarray)
			return vertarray;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		// Vertex information
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex vertex;
			glm::vec3 vector;
			// Position
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;
			// Normal
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;

			// Tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			// biTangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;

			// TextureCoord
			// assimp let vertex has 8 max texture 0~7
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texcoords = vec;
			}
			else
				vertex.texcoords = glm::vec2(0.0f, 0.0f);

			vertices.emplace_back(vertex);
		}
		// Index information
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; ++j)
				indices.emplace_back(face.mIndices[j]);
		}
		
		vertarray = VertexArray::Create(meshName);
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));

		vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

		// makeIndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		Ref<Shader> shader;
		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/Blinn-Phong.hlsl");
		}
		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent" },
			  {ShaderDataType::Float3,  "a_BiTangent"}
			}, shader
		));

		vertarray->AddVertexBuffer(vertexBuffer);
		vertarray->SetIndexBuffer(indexBuffer);

		//unbind
		vertarray->UnBind();
		MeshLibrary::Set(meshName, vertarray);
		return vertarray;
	}
	void SetVertexBoneDataDefault(AnimatedVertex& vertex)
	{
		for (int i = 0; i < 4; i++)
		{
			vertex.boneIDs[i] = -1;
			vertex.weights[i] = 0.0f;
		}
	}
	void SetVertexBoneData(AnimatedVertex& vertex, int boneID, float weight)
	{
		if(vertex.boneIDs)
		for (int i = 0; i < 4; ++i)
		{
			if (vertex.boneIDs[i] < 0)
			{
				vertex.boneIDs[i] = boneID;
				vertex.weights[i] = weight;
				break;
			}
		}
	}
	void ExtractBoneWeightForVertices(std::vector<AnimatedVertex>& vertices, aiMesh* mesh, const aiScene* scene, BoneStructure& boneStructure,DynamicMeshComponent& comp)
	{
		for (int boneindex = 0; boneindex < mesh->mNumBones; ++boneindex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneindex]->mName.C_Str();

			auto& boneInfoMap = boneStructure.m_OffsetMatMap;
			auto& boneCount = boneStructure.boneCount;
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offsetMatrix = Utils::ConvertToGlm(mesh->mBones[boneindex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				comp.m_OffsetMatrix[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
				comp.m_OffsetMatrix[boneName] = boneInfoMap[boneName];
			}
				

			QCAT_ASSERT(boneID != -1);
			auto weights = mesh->mBones[boneindex]->mWeights;
			int numWeights = mesh->mBones[boneindex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				QCAT_ASSERT(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}
	Ref<VertexArray> ModelLoader::ProcessDynamicMesh(aiNode* node, aiMesh* mesh, const aiScene* scene, DynamicMeshComponent& comp, BoneStructure& boneStructure, unsigned index)
	{
		// Check is there mesh in library
		std::string meshName = mesh->mName.C_Str();
		if (meshName == "")
		{
			meshName = node->mName.C_Str();
			meshName += "_Mesh";
		}
		if (index != 0)
			meshName = meshName + "_" + std::to_string(index);
		Ref<VertexArray> vertarray;
		/*Ref<VertexArray> vertarray = MeshLibrary::Load(meshName);
		if (vertarray)
			return vertarray;*/

		std::vector<AnimatedVertex> vertices;
		std::vector<uint32_t> indices;
		// Vertex information
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			AnimatedVertex vertex;
			SetVertexBoneDataDefault(vertex);
			glm::vec3 vector;
			// Position
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;
			// Normal
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;

			// Tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			// biTangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;

			// TextureCoord
			// assimp let vertex has 8 max texture 0~7
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texcoords = vec;
			}
			else
				vertex.texcoords = glm::vec2(0.0f, 0.0f);

			vertices.emplace_back(vertex);
		}
		// Index information
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; ++j)
				indices.emplace_back(face.mIndices[j]);
		}
		ExtractBoneWeightForVertices(vertices, mesh, scene, boneStructure,comp);

		vertarray = VertexArray::Create(meshName);
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(AnimatedVertex));

		vertexBuffer->SetData(vertices.data(), sizeof(AnimatedVertex) * vertices.size());

		// makeIndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		Ref<Shader> shader;
		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/Blinn-Phong_Animation.hlsl");
		}
		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"},
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent" },
			  { ShaderDataType::Float3, "a_BiTangent"},
			  { ShaderDataType::Int4,	"a_BoneIDs"},
			  {	ShaderDataType::Float4,	"a_Weights"}
			}, shader
		));

		vertarray->AddVertexBuffer(vertexBuffer);
		vertarray->SetIndexBuffer(indexBuffer);

		//unbind
		vertarray->UnBind();
		MeshLibrary::Set(meshName, vertarray);
		return vertarray;
	}
	Material ModelLoader::ProcessMaterial(const std::string& path, const aiScene* scene, aiMesh* mesh)
	{
		size_t index =  path.find_last_of("/\\")+1;
		std::string dir = path.substr(0, index);
		Material material;
		std::string fullpath;
		aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
		Sampler_Desc desc;
		// Diffuse Texture
		if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir+path.data;
				if (std::filesystem::exists(fullpath))
				{
					material.m_DiffuseTexture = TextureLibrary::Load(fullpath, desc);
				}
			}
		}
		// Specular Texture
		if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
				if (std::filesystem::exists(fullpath))
				{
					material.m_SpecularTexture = TextureLibrary::Load(fullpath, desc);
					material.m_MetallicTexture = TextureLibrary::Load(fullpath, desc);
				}
			}
		}
		// Ambient Texture
		if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
				if (std::filesystem::exists(fullpath))
				{
					material.m_MetallicTexture = TextureLibrary::Load(fullpath, desc);
				}
			}
		}
		// Emissive Texture
		if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Height Texture
		if (aimaterial->GetTextureCount(aiTextureType_HEIGHT) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
				if (std::filesystem::exists(fullpath))
				{
					material.m_NormalMapTexture = TextureLibrary::Load(fullpath, desc);
				}
			}
		}
		// Normal Texture
		if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
				if (std::filesystem::exists(fullpath))
				{
					material.m_NormalMapTexture = TextureLibrary::Load(fullpath, desc);
				}
			}
		}
		// Shininess Texture
		if (aimaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Opacity Texture (Transparent)
		if (aimaterial->GetTextureCount(aiTextureType_OPACITY) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Displacement Texture
		if (aimaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// LightMap Texture
		if (aimaterial->GetTextureCount(aiTextureType_LIGHTMAP) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// PBR Texture
		// BaseColor Texture
		if (aimaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Emissive Color Texture
		if (aimaterial->GetTextureCount(aiTextureType_EMISSION_COLOR) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_EMISSION_COLOR, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Metalness Texture
		if (aimaterial->GetTextureCount(aiTextureType_METALNESS) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Roughness Texture
		if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Ambient Occlusion Texture
		if (aimaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}
		// Unknown
		// Occlusion Texture
		if (aimaterial->GetTextureCount(aiTextureType_UNKNOWN) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
			}
		}

		return material;
	}
}