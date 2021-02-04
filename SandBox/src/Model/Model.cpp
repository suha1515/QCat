#include "Model.h"

namespace QCat
{
	Model::Model(const char* path)
	{
		LoadModel(path);
	}
	void Model::Draw(const Ref<Shader>& shader)
	{
		for (uint32_t i = 0; i < meshes.size(); ++i)
			meshes[i].Draw(shader);

	}
	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;
		uint32_t flag;
		flag |= aiProcess_Triangulate;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			flag |= aiProcess_FlipUVs;
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
			std::vector<Mesh::Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Mesh::Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
		// TODO: Shader split..?
		return Mesh(vertices, indices, textures,nullptr);
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
		}
	}
}