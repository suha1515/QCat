#include "Model.h"
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
	Model::Model(const char* path)
	{
		LoadModel(path);
	}
	const glm::mat4 Model::GetTransform()
	{
		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);

		return transform;
	}
	void Model::Draw()
	{
			
	}
	Ref<Model> Model::Create(const char* path)
	{
		return CreateRef<Model>(path);
	}
	
	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;

		//if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		//	flag |= aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate |  
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace | 
			aiProcess_GenNormals |
			aiProcess_MakeLeftHanded |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "Error:Assimp:";
			QCAT_ASSERT(false, error+importer.GetErrorString());
		}
		this->path = path.substr(0, path.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);

	}
	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		// if node has meshes, Node has a meshIndex and Scene has a real mesh so
		// if node want to acess its own meshes , pass the nodes meshIndex to scene->mMeshes
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(node,mesh, scene));
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}

	}
	ModelMesh Model::ProcessMesh(aiNode* node,aiMesh* mesh, const aiScene* scene)
	{
		glm::mat4 transform = Utils::ConvertToGlm(node->mTransformation);

		std::vector<ModelMesh::Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<ModelMesh::Texture> textures;
	
		// Vertex information
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			ModelMesh::Vertex vertex;
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
		Material mat;
		// Material info
		// like node , mesh only has material index so to get real material infomation we need to index scene's mMaterial
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiString name = material->GetName();

			/*for (int i = 0; i < 19; ++i)
			{
				unsigned int numTextures = material->GetTextureCount((aiTextureType)i);
				bool aa = true;
			}
				aiColor3D color(0.f, 0.f, 0.f);
				bool success = false;
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
				{
					success = true;
				}
				if(aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_TRANSPARENT, color))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_REFLECTIVE, color))
				{
					success = true;
				}
				float value = 0.0f;
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_OPACITY, value))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_TRANSPARENCYFACTOR, value))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_BUMPSCALING, value))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_SHININESS, value))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_REFLECTIVITY, value))
				{
					success = true;
				}
				if (aiReturn_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, value))
				{
					success = true;
				}*/
			//aiString str;
			//std::string texturePath;
			//if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> diffTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(diffTexture, Material::TextureType::Diffuse);
			//}
			//if (material->GetTexture(aiTextureType_SPECULAR, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> specularTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(specularTexture, Material::TextureType::Specular);
			//}
			//if (material->GetTexture(aiTextureType_HEIGHT, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> normalTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(normalTexture, Material::TextureType::Normal);
			//}
			//if (material->GetTexture(aiTextureType_AMBIENT, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_EMISSION_COLOR, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_EMISSIVE, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_NORMALS, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> normalTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(normalTexture, Material::TextureType::Normal);
			//}
			//if (material->GetTexture(aiTextureType_REFLECTION, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_OPACITY, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_SHININESS, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> roughnessTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(roughnessTexture, Material::TextureType::Roughness);
			//}
			//if (material->GetTexture(aiTextureType_UNKNOWN, 0, &str) == aiReturn_SUCCESS)
			//{
			//	QCAT_CORE_ASSERT(false, "Find other imgageType");
			//}
			//if (material->GetTexture(aiTextureType_METALNESS, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> metallicTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(metallicTexture, Material::TextureType::Metallic);
			//}
			////roughness
			//if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> roughnessTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(roughnessTexture, Material::TextureType::Roughness);
			//}
			////ao
			//if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str) == aiReturn_SUCCESS)
			//{
			//	texturePath = path + '/' + str.C_Str();
			//	//texturePath = str.C_Str();

			//	Ref<Texture2D> ambientOcclusionTexture = TextureLibrary::Load(texturePath);
			//	mat.SetTexture(ambientOcclusionTexture, Material::TextureType::AmbientOcclusion);
			//}
		}
		Ref<Shader> shader;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = ShaderLibrary::Load("lightShader","Asset/shaders/glsl/Blinn-phong.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl", "Asset/shaders/hlsl/BlinnAndPhong_PS.hlsl");
		}
		// TODO: Shader split..?
		return ModelMesh(transform,vertices, indices, shader);
	}
	std::vector<ModelMesh::Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeMame)
	{
		std::vector<ModelMesh::Texture> textures;
		for (uint32_t i = 0; i < mat->GetTextureCount(type); ++i)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			ModelMesh::Texture texture;
			std::string texturePath = path + '/' + str.C_Str();
			texture.texture = TextureLibrary::Load(texturePath);
			texture.type = typeMame;
			textures.push_back(texture);
		}
		return textures;
	}
	Entity ModelLoader::LoadModel(const char* path, const Ref<Scene>& pScene)
	{
		return LoadModelEntity(path, pScene);
	}
	Entity ModelLoader::LoadModelEntity(const std::string& path, const Ref<Scene>& pScene)
	{
		Assimp::Importer importer;

		//if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		//	flag |= aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_MakeLeftHanded |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "Error:Assimp:";
			QCAT_ASSERT(false, error + importer.GetErrorString());
		}

		bool IsAnimMesh = scene->HasAnimations();
		Animator animator;

		Entity rootNode;

		if (IsAnimMesh)
		{
			rootNode =  ProcessNodeEntity(scene->mRootNode, scene, pScene, &animator, nullptr);
			auto& anim = rootNode.AddComponent<AnimatorComponent>();
			anim = animator;
			anim.animator.Initialize(scene, rootNode);
		}
		else
			rootNode = ProcessNodeEntity(scene->mRootNode, scene, pScene, nullptr, nullptr);
		return rootNode;
	}
	Entity ModelLoader::ProcessNodeEntity(aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Animator* animator, Entity* parentEntity )
	{
		// if node has meshes, Node has a meshIndex and Scene has a real mesh so
		// if node want to acess its own meshes , pass the nodes meshIndex to scene->mMeshes
		std::string nodeName = node->mName.C_Str();
		Entity entity = pScene->CreateEntity(nodeName,parentEntity);
		glm::mat4 transform = Utils::ConvertToGlm(node->mTransformation);
		glm::vec3 scale, translation, skew;
		glm::quat rotation;
		glm::vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);
		entity.GetComponent<TransformComponent>().Rotation = glm::eulerAngles(rotation);
		entity.GetComponent<TransformComponent>().Scale = scale;
		entity.GetComponent<TransformComponent>().Translation = translation;

		std::vector<Ref<VertexArray>> vertexArrays;
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			if (animator!=nullptr)
			{
				vertexArrays.push_back(ProcessAnimatedMeshEntity(node, mesh, scene, *animator));
			}
			else
				vertexArrays.push_back(ProcessMeshEntity(node, mesh, scene));
		}
		if (node->mNumMeshes != 0)
		{
			auto& meshes = entity.AddComponent<MeshComponent>();
			meshes.vertexArray = vertexArrays;
			entity.AddComponent<MaterialComponent>();
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNodeEntity(node->mChildren[i], scene, pScene,animator,&entity);
		} 
		return entity;
	}
	Ref<VertexArray> ModelLoader::ProcessMeshEntity(aiNode* node, aiMesh* mesh, const aiScene* scene)
	{
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
		Ref<VertexArray> vertarray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));

		vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

		// makeIndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		Ref<Shader> shader;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = ShaderLibrary::Load("lightShader", "Asset/shaders/glsl/Blinn-phong.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/Blinn-Phong.hlsl");
		}
		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent" },
			  {	ShaderDataType::Float3,  "a_BiTangent"},
			}, shader
		));

		vertarray->AddVertexBuffer(vertexBuffer);
		vertarray->SetIndexBuffer(indexBuffer);

		//unbind
		vertarray->UnBind();
		MeshLibrary::Set(mesh->mName.C_Str(), vertarray);
		return vertarray;
	}
	Entity ModelLoader::ProcessAnimatedNodeEntity(aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity)
	{
		return Entity();
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

	void ExtractBoneWeightForVertices(std::vector<AnimatedVertex>& vertices, aiMesh* mesh, const aiScene* scene, Animator& animator)
	{
		for (int boneindex = 0; boneindex < mesh->mNumBones; ++boneindex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneindex]->mName.C_Str();
			auto& boneInfoMap = animator.m_OffsetMatMap;
			auto& boneCount = animator.m_BoneCount;
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offsetMatrix = Utils::ConvertToGlm(mesh->mBones[boneindex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
				boneID = boneInfoMap[boneName].id;

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
	Ref<VertexArray> ModelLoader::ProcessAnimatedMeshEntity(aiNode* node, aiMesh* mesh, const aiScene* scene, Animator& animator)
	{

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
		
		ExtractBoneWeightForVertices(vertices, mesh, scene,animator);

		Ref<VertexArray> vertarray = VertexArray::Create(mesh->mName.C_Str());
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(AnimatedVertex));

		vertexBuffer->SetData(vertices.data(), sizeof(AnimatedVertex) * vertices.size());

		// makeIndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		Ref<Shader> shader;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = ShaderLibrary::Load("lightShader", "Asset/shaders/glsl/Blinn-phong_Animation.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/Blinn-phong_Animation.hlsl");
		}
		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent" },
			  {ShaderDataType::Float3,  "a_BiTangent"},
			{	ShaderDataType::Int4,"a_BoneIDs"},
			  {	ShaderDataType::Float4,"a_Weights"}
			}, shader
		));

		vertarray->AddVertexBuffer(vertexBuffer);
		vertarray->SetIndexBuffer(indexBuffer);

		//unbind
		vertarray->UnBind();
		MeshLibrary::Set(vertarray->GetMeshName(), vertarray);
		return vertarray;
	}
	
}