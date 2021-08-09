#include "qcpch.h"
#include "Mesh.h"
#include "RenderAPI.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
	void PorcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene,unsigned int index=0)
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
		if (!vertarray)
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

			Ref<VertexArray> vertarray = VertexArray::Create(meshName);
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
		}
	}
	void ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			PorcessMesh(node, mesh, scene);
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}
	void ModelLibrary::Load_(const std::string& modelPath)
	{
		for (auto& list : this->modelPath)
		{
			if (modelPath == list)
				return;
		}
		this->modelPath.push_back(modelPath);
		unsigned int flag;
		flag = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_MakeLeftHanded |
			aiProcess_FlipWindingOrder;

		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
			flag |= aiProcess_FlipUVs;

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(modelPath,flag);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "Error:Assimp:";
			QCAT_ASSERT(false, error + importer.GetErrorString());
		}
		ProcessNode(scene->mRootNode, scene);
	}
}