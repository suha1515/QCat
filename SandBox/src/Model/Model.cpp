#include "Model.h"

namespace QCat
{
	Model::Model(const char* path)
	{
		LoadModel(path);
	}
	void Model::Draw(const Ref<Shader>& shader)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		shader->SetMat4("u_Transform", transform, ShaderType::VS);
		shader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
		shader->UpdateBuffer();
		//material.Bind();
		for (uint32_t i = 0; i < meshes.size(); ++i)
			meshes[i].Draw(shader, transform);
	}
	Ref<Model> Model::Create(const char* path)
	{
		return CreateRef<Model>(path);
	}
	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;
		uint32_t flag;
		flag |= aiProcess_Triangulate | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded| aiProcess_FlipUVs;
		//if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		//	flag |= aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(path, flag);

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
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		// and also node can have child nodes, do this procedure recursively
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}

	}
	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Mesh::Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Mesh::Texture> textures;
		Material mat;
		// Vertex information
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			Mesh::Vertex vertex;
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

		// Material info
		// like node , mesh only has material index so to get real material infomation we need to index scene's mMaterial
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			//std::vector<Mesh::Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

			//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			//std::vector<Mesh::Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			//std::vector<Mesh::Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			aiString str;
			std::string texturePath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == aiReturn_SUCCESS)
			{
				texturePath = path + '/' + str.C_Str();
				Ref<Texture2D> diffTexture = TextureLibrary::Load(texturePath);
				mat.SetTexture(diffTexture, Material::MaterialType::Diffuse);
			}
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &str) == aiReturn_SUCCESS)
			{
				texturePath = path + '/' + str.C_Str();
				Ref<Texture2D> specularTexture = TextureLibrary::Load(texturePath);
				mat.SetTexture(specularTexture, Material::MaterialType::Specular);
			}
			if (material->GetTexture(aiTextureType_HEIGHT, 0, &str) == aiReturn_SUCCESS)
			{
				texturePath = path + '/' + str.C_Str();
				Ref<Texture2D> normalTexture = TextureLibrary::Load(texturePath);
				mat.SetTexture(normalTexture, Material::MaterialType::NormalMap);
			}
			this->material = mat;
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
		return Mesh(vertices, indices, mat, shader);
	}
	std::vector<Mesh::Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeMame)
	{
		std::vector<Mesh::Texture> textures;
		for (uint32_t i = 0; i < mat->GetTextureCount(type); ++i)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			Mesh::Texture texture;
			std::string texturePath = path + '/' + str.C_Str();
			texture.texture = TextureLibrary::Load(texturePath);
			texture.type = typeMame;
			textures.push_back(texture);
		}
		return textures;
	}
}