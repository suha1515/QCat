#include "qcpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace QCat
{
	// glm::vec operator
	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v)
	{
		// flow is alow to insert data like [1,2,3,4]
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		:m_Scene(scene)
	{

	}
	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		// after begin map key and value write
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUID(); // TODO: Entity ID goes in here
		
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; //Tag Component

			//write value at YAML
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap; //Tag Component
		}
		if (entity.HasComponent<RelationShipComponent>())
		{
			auto& relationshipComp = entity.GetComponent<RelationShipComponent>();
			out << YAML::Key << "RelationShipComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Parent" << YAML::Value << relationshipComp.parent;
			out << YAML::Key << "PrevSibling" << YAML::Value << relationshipComp.prevSibling;
			out << YAML::Key << "NextSibling" << YAML::Value << relationshipComp.nextSibling;
			out << YAML::Key << "FirstChild" << YAML::Value << relationshipComp.firstChild;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;// TransformComponent;

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; //TransformComponent;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType"   << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFov"   << YAML::Value << camera.GetPerspectiveVerticalFov();
			out << YAML::Key << "PerspectiveNear"  << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar"   << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthoGraphicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar"  << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent

 		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent" << YAML::Value << YAML::BeginSeq;
			auto& meshComponent = entity.GetComponent<MeshComponent>();
			for (auto& mesh : meshComponent.GetMeshes())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Meshname" << YAML::Value << mesh->GetMeshName();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		if (entity.HasComponent<MaterialComponent>())
		{
			auto& materialComponent = entity.GetComponent<MaterialComponent>();
			auto& material = materialComponent.material;
			out << YAML::Key << "MaterialComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Albedo" << YAML::Value << material.diffuse;
			out << YAML::Key << "Specular" << YAML::Value << material.specular;
			out << YAML::Key << "Ambient" << YAML::Value << material.ambient;
			out << YAML::Key << "Shininess" << YAML::Value << material.shininess;
			out << YAML::Key << "Metallic" << YAML::Value << material.metallic;
			out << YAML::Key << "Roughness" << YAML::Value << material.roughness;
			out << YAML::Key << "AmbientOcclusion" << YAML::Value << material.ao;

			out << YAML::Key << "AlbedoTexture" << YAML::Value << (material.m_DiffuseTexture != nullptr ? material.m_DiffuseTexture->GetTexturePath() : "none");
			out << YAML::Key << "AlbedoTextureSampler" << (material.m_DiffuseTexture != nullptr ? material.m_DiffuseTexture->GetSamplerSignature() : "none");

			out << YAML::Key << "NormalTexture" << YAML::Value << (material.m_NormalMapTexture != nullptr ? material.m_NormalMapTexture->GetTexturePath() : "none");
			out << YAML::Key << "NormalTextureSampler" << (material.m_NormalMapTexture != nullptr ? material.m_NormalMapTexture->GetSamplerSignature() : "none");

			out << YAML::Key << "SpecularTexture" << YAML::Value << (material.m_SpecularTexture != nullptr ? material.m_NormalMapTexture->GetTexturePath() : "none");
			out << YAML::Key << "SpecularTextureSampler" << (material.m_SpecularTexture != nullptr ? material.m_SpecularTexture->GetSamplerSignature() : "none");

			out << YAML::Key << "MetallicTexture" << YAML::Value << (material.m_MetallicTexture != nullptr ? material.m_MetallicTexture->GetTexturePath() : "none");
			out << YAML::Key << "MetallicTextureSampler" << (material.m_MetallicTexture != nullptr ? material.m_MetallicTexture->GetSamplerSignature() : "none");

			out << YAML::Key << "RoughnessTexture" << YAML::Value << (material.m_RoughnessTexture != nullptr ? material.m_RoughnessTexture->GetTexturePath() : "none");
			out << YAML::Key << "RoughnessTextureSampler" << (material.m_RoughnessTexture != nullptr ? material.m_RoughnessTexture->GetSamplerSignature() : "none");

			out << YAML::Key << "AmbientOcclusionTexture" << YAML::Value << (material.m_AmbientOcclusionTexture != nullptr ? material.m_AmbientOcclusionTexture->GetTexturePath() : "none");
			out << YAML::Key << "AmbientOcclusionTextureSampler" << (material.m_AmbientOcclusionTexture != nullptr ? material.m_AmbientOcclusionTexture->GetSamplerSignature() : "none");

			out << YAML::EndMap;

		}
		if (entity.HasComponent<LightComponent>())
		{
			auto& lightComponent = entity.GetComponent<LightComponent>();
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "LightColor";
			out << YAML::BeginMap;
			out << YAML::Key << "Diffuse" << YAML::Value << lightComponent.diffuse;
			out << YAML::Key << "Ambient" << YAML::Value << lightComponent.ambient;
			out << YAML::Key << "Specular" << YAML::Value << lightComponent.specular;
			out << YAML::EndMap;
			out << YAML::Key << "Directional Light";
			out << YAML::BeginMap;
			out << YAML::Key << "LightDirection" << YAML::Value << lightComponent.lightDirection;
			out << YAML::EndMap;
			out << YAML::Key << "Point Light";
			out << YAML::BeginMap;
			out << YAML::Key << "Constant" << YAML::Value << lightComponent.constant;
			out << YAML::Key << "Quadratic" << YAML::Value << lightComponent.quadratic;
			out << YAML::Key << "Linear" << YAML::Value << lightComponent.linear;
			out << YAML::Key << "Radius" << YAML::Value << lightComponent.radius;
			out << YAML::EndMap;
			out << YAML::Key << "Spot Light";
			out << YAML::BeginMap;
			out << YAML::Key << "Cutoff" << YAML::Value << lightComponent.cutoff;
			out << YAML::Key << "OuterCutoff" << YAML::Value << lightComponent.outerCutOff;
			out << YAML::EndMap;
			out << YAML::Key << "Type" << YAML::Value << static_cast<int>(lightComponent.type);
			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Entity
	}
	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		
		// begin Map
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled"; // Key and Value
		//Model List
		out << YAML::Key << "ModelPath" << YAML::Value << YAML::BeginSeq;
		for (auto& path : ModelLibrary::GetModelList())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Path" << YAML::Value << path;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		//Texture Info
		out << YAML::Key << "TexturePath" << YAML::Value << YAML::BeginSeq;
		for (auto& path : TextureLibrary::GetTexturePathList())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Path" << YAML::Value << path;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Sequence is kind of array
		// after BeginSeq loop for every entity
		//m_Scene->m_Registry.each([&](auto entityID)
		//	{
		//		Entity entity = { entityID,m_Scene.get() };
		//		if (!entity)
		//			return;
		//		// if entity is validate, serialize it
		//		SerializeEntity(out, entity);
		//	}
		//);
		for (const auto& iter : m_Scene->m_entityMap)
		{
			SerializeEntity(out,iter.second);
		}
		// End Sequence
		out << YAML::EndSeq;
		out << YAML::EndMap;

		// output it into file
		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implementd
		QCAT_CORE_ASSERT(false);
	}
	bool SceneSerializer::DeSerialize(const std::string& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath);
		// Find Data Scene (Node data)
		if (!data["Scene"])
			return false;
		// Data as string
		std::string sceneName = data["Scene"].as<std::string>();
		QCAT_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		// Model Load
		auto modelpath = data["ModelPath"];
		if (modelpath)
		{
			for (auto list : modelpath)
			{
				std::string path = list["Path"].as<std::string>();
				ModelLibrary::LoadModel(path);
			}
		}

		// Texture Load
		/*auto texturepath = data["TexturePath"];
		if (texturepath)
		{
			for (auto list : texturepath)
			{
				std::string path = list["Path"].as<std::string>();
				Sampler_Desc samp;
				TextureLibrary::Load(path, samp);
			}
		}*/
	
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				QCAT_CORE_TRACE("Deserialized entity with ID = {0}, name ={1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(name,uuid); // TODO : make function that take uuid

				auto rccomp = entity["RelationShipComponent"];
				if (rccomp)
				{
					auto& rc = deserializedEntity.GetComponent<RelationShipComponent>();
					rc.parent = rccomp["Parent"].as<uint32_t>();
					rc.prevSibling = rccomp["PrevSibling"].as<uint32_t>();
					rc.nextSibling = rccomp["NextSibling"].as<uint32_t>();
					rc.firstChild = rccomp["FirstChild"].as<uint32_t>();
				}
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities alawys have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation    = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale       = transformComponent["Scale"].as<glm::vec3>();
					tc.changed = true;
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& mc = deserializedEntity.AddComponent<MeshComponent>();
					for (auto meshName : meshComponent)
					{
						std::string name = meshName["Meshname"].as<std::string>();
						mc.AddMesh(name);
					}
				}

				auto materialComponent = entity["MaterialComponent"];
				if (materialComponent)
				{
					auto& cc = deserializedEntity.AddComponent<MaterialComponent>();

					glm::vec3 value;
					cc.material.diffuse = materialComponent["Albedo"].as<glm::vec3>();
					cc.material.specular = materialComponent["Specular"].as<glm::vec3>();
					cc.material.ambient = materialComponent["Ambient"].as<glm::vec3>();

					cc.material.shininess = materialComponent["Shininess"].as<float>();
					cc.material.metallic = materialComponent["Metallic"].as<float>();
					cc.material.roughness = materialComponent["Roughness"].as<float>();
					cc.material.ao = materialComponent["AmbientOcclusion"].as<float>();

					std::string texturepath;
					std::string samplerSignature;
					texturepath = materialComponent["AlbedoTexture"].as<std::string>();
					samplerSignature = materialComponent["AlbedoTextureSampler"].as<std::string>();

					if (texturepath != "none")
					{
						Sampler_Desc desc = SamplerState::MakeSamplerDesc(samplerSignature);
						cc.material.SetTexture(texturepath,desc, Material::TextureType::Diffuse);
					}
					 
					texturepath = materialComponent["NormalTexture"].as<std::string>();
					samplerSignature = materialComponent["NormalTextureSampler"].as<std::string>();

					if (texturepath != "none")
					{
						Sampler_Desc desc = SamplerState::MakeSamplerDesc(samplerSignature);
						cc.material.SetTexture(texturepath, desc, Material::TextureType::Normal);
					}

					texturepath = materialComponent["SpecularTexture"].as<std::string>();
					samplerSignature = materialComponent["SpecularTextureSampler"].as<std::string>();

					if (texturepath != "none")
					{
						Sampler_Desc desc = SamplerState::MakeSamplerDesc(samplerSignature);
						cc.material.SetTexture(texturepath, desc, Material::TextureType::Specular);
					}

					texturepath = materialComponent["MetallicTexture"].as<std::string>();
					samplerSignature = materialComponent["MetallicTextureSampler"].as<std::string>();

					if (texturepath != "none")
					{
						Sampler_Desc desc = SamplerState::MakeSamplerDesc(samplerSignature);
						cc.material.SetTexture(texturepath, desc, Material::TextureType::Metallic);
					}

					texturepath = materialComponent["RoughnessTexture"].as<std::string>();
					samplerSignature = materialComponent["RoughnessTextureSampler"].as<std::string>();

					if (texturepath != "none")
					{
						Sampler_Desc desc = SamplerState::MakeSamplerDesc(samplerSignature);
						cc.material.SetTexture(texturepath, desc, Material::TextureType::Roughness);
					}

					texturepath = materialComponent["AmbientOcclusionTexture"].as<std::string>();
					samplerSignature = materialComponent["AmbientOcclusionTextureSampler"].as<std::string>();

					if (texturepath != "none")
					{
						Sampler_Desc desc = SamplerState::MakeSamplerDesc(samplerSignature);
						cc.material.SetTexture(texturepath, desc, Material::TextureType::AmbientOcclusion);
					}
				}
				auto lightcomp = entity["LightComponent"];
				if (lightcomp)
				{
					auto& lc = deserializedEntity.AddComponent<LightComponent>();
					auto value = lightcomp["LightColor"];
					lc.diffuse = value["Diffuse"].as<glm::vec3>();
					lc.ambient = value["Ambient"].as<glm::vec3>();
					lc.specular = value["Specular"].as<glm::vec3>();

					value = lightcomp["Directional Light"];
					lc.lightDirection = value["LightDirection"].as<glm::vec3>();
					value = lightcomp["Point Light"];
					lc.constant = value["Constant"].as<float>();
					lc.quadratic = value["Quadratic"].as<float>();
					lc.linear = value["Linear"].as<float>();
					lc.radius = value["Radius"].as<float>();
					value = lightcomp["Spot Light"];
					lc.cutoff = value["Cutoff"].as<float>();
					lc.outerCutOff = value["OuterCutoff"].as<float>();

					switch (lightcomp["Type"].as<uint32_t>())
					{
					case 0:
						lc.type = LightComponent::LightType::Directional;
						break;
					case 1:
						lc.type = LightComponent::LightType::Point;
						break;
					case 2:
						lc.type = LightComponent::LightType::Spot;
						break;
					default:
						break;
					}	
				}

;				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					
					auto cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFov(cameraProps["PerspectiveFov"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthoGraphicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}

				
			}
		}

		return true;
	}
	bool SceneSerializer::DeSerializeRuntime(const std::string& filepath)
	{
		// Not implementd
		QCAT_CORE_ASSERT(false);
		return false;
	}
}