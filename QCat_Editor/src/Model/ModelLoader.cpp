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
	struct AniamtedVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		int boneIDs[4];
		float weights[4];
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
	Entity ModelLoader::LoadModel(const char* path, const Ref<Scene>& pScene)
	{
		ModelLibrary::GetModelList().push_back(path);
		return LoadModelEntity(path, pScene);
	}
	Entity ModelLoader::LoadModelEntity(const std::string& path, const Ref<Scene>& pScene)
	{
		Assimp::Importer importer;

		unsigned int flag;
		flag = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_MakeLeftHanded |
			aiProcess_FlipWindingOrder;

		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
			flag |= aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(path,
			flag);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "Error:Assimp:";
			QCAT_ASSERT(false, error + importer.GetErrorString());
		}

		return ProcessNodeEntity(path,scene->mRootNode, scene, pScene, nullptr);
	}
	Entity ModelLoader::ProcessNodeEntity(const std::string& path, aiNode* node, const aiScene* scene, const Ref<Scene>& pScene, Entity* parentEntity)
	{
		// if node has meshes, Node has a meshIndex and Scene has a real mesh so
		// if node want to acess its own meshes , pass the nodes meshIndex to scene->mMeshes
		std::string nodeName = node->mName.C_Str();
		Entity entity = pScene->CreateEntity(nodeName);
		entity.SetParent(parentEntity);

		glm::mat4 transform = Utils::ConvertToGlm(node->mTransformation);
		glm::vec3 scale, translation, skew;
		glm::quat rotation;
		glm::vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);
		entity.GetComponent<TransformComponent>().Rotation = glm::eulerAngles(rotation);
		entity.GetComponent<TransformComponent>().Scale = scale;
		entity.GetComponent<TransformComponent>().Translation = translation;
		
		std::vector<Ref<VertexArray>> vertexArrays;
		Material material;
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			vertexArrays.push_back(ProcessMesh(node, mesh, scene));
			if (i < 1)
			{
				material = ProcessMaterial(path, scene, mesh);
			}	
		}
		if(node->mNumMeshes>1)
			QCAT_CORE_WARN("node has mesh morethan one");
		if (node->mNumMeshes != 0)
		{
			entity.AddComponent<MeshComponent>().vertexArray = vertexArrays;
			entity.AddComponent<MaterialComponent>().material = material;
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNodeEntity(path,node->mChildren[i], scene, pScene, &entity);
		}
		return entity;
	}
	Ref<VertexArray> ModelLoader::ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene)
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
		std::string meshName = mesh->mName.C_Str();
		if (meshName == "")
		{
			meshName = node->mName.C_Str();
			meshName += "_Mesh";
		}
			
		Ref<VertexArray> vertarray = VertexArray::Create(meshName);
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
				material.m_DiffuseTexture = TextureLibrary::Load(fullpath, desc);
			}
		}
		// Specular Texture
		if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
				material.m_SpecularTexture = TextureLibrary::Load(fullpath, desc);
				material.m_MetallicTexture = TextureLibrary::Load(fullpath, desc);
			}
		}
		// Ambient Texture
		if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
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
			}
		}
		// Normal Texture
		if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
			{
				fullpath = dir + path.data;
				material.m_NormalMapTexture = TextureLibrary::Load(fullpath, desc);
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