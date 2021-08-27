#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENT
#include <glm/gtx/quaternion.hpp>


#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "QCat/Renderer/Mesh.h"
#include "QCat/Renderer/Material.h"
#include "Entity.h"
#include "QCat/Math/Math.h"
#include "QCat/Animation/Animator.h"

namespace QCat
{
	struct BasicComponent
	{
		Entity owner;
	};
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string & tag)
			:Tag(tag) {}
	};
	struct GuidComponent
	{
		uint32_t uid;
		GuidComponent() = default;
	};
	struct TransformComponent
	{
		glm::mat4 parentMatrix = glm::mat4(1.0f);
		glm::vec3 Translation = { 0.0f,0.0f,0.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };
		bool isClicked = false;
		bool changed = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			:Translation(translation) {}

		void SetTranslation(const glm::vec3& translation)
		{
			if (Translation != translation)
			{
				Translation = translation;
				changed = true;
			}	
		}
		void SetRotation(const glm::vec3& rotation)
		{
			if (Rotation != rotation)
			{
				Rotation = rotation;
				changed = true;
			}
		}
		void SetScale(const glm::vec3& scale)
		{
			if (Scale != scale)
			{
				Scale = scale;
				changed = true;
			}
		}
		void SetTransform(const glm::mat4& transform)
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			Translation = translation;
			Rotation = rotation;
			Scale = scale;
			changed = true;
		}
		glm::mat4 GetTransform() const
		{
			return parentMatrix * GetLocalTransform();
		}
		glm::mat4 GetLocalTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			const glm::mat4 localmatrix = glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
			return localmatrix;
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f,1.0f,1.0f,1.f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 & color)
			:Color(color) {}
	};
	struct CameraComponent
	{
		SceneCamera   Camera;
		bool Primary = true; // TODO:: think about moving to Scene
		bool FixedAspectRatio = false; 

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void(*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() {return  static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
	struct MeshInformation
	{
		Ref<VertexArray> vertexArray;
		std::string materialName;
	};
	struct MeshComponent
	{
		Ref<VertexArray> vertexArray;
		bool isDynamic = false;
		MeshComponent() = default;
		MeshComponent(const std::string& meshName)
		{
			vertexArray = MeshLibrary::Load(meshName);
		}
		MeshComponent(const std::string& meshName,Ref<VertexArray>& vertexarray)
		{
			MeshLibrary::Set(meshName, vertexarray);
			vertexArray = std::move(vertexarray);
		}
		void AddMesh(const std::string& meshName)
		{
			auto mesh = MeshLibrary::Load(meshName);
			if (mesh != nullptr)
			{
				vertexArray = mesh;
			}
			else
				QCAT_CORE_ERROR("There is no mesh name '{0}'", meshName);
		}
		void AddMesh(Ref<VertexArray>& vertexarray)
		{
			vertexArray = vertexarray;
		}
		Ref<VertexArray>& GetMesh() { return vertexArray; }
	};

	// DynamicMesh has a Node that effect mesh
	struct DynamicMeshComponent
	{
		Ref<VertexArray> vertexArray;
		//std::set<std::string> m_boneName;
		std::unordered_map<std::string, BoneInfo> m_OffsetMatrix;
		//std::vector<uint32_t> m_nodes;
		std::vector<std::pair<uint32_t, std::string>> m_nodes;

		std::string modelPath;
		DynamicMeshComponent() = default;
		//DynamicMeshComponent()
		void AddMesh(const std::string& meshName)
		{
			auto mesh = MeshLibrary::Load(meshName);
			if (mesh != nullptr)
				vertexArray = mesh;
			else
				QCAT_CORE_ERROR("There is no mesh name '{0}'", meshName);
		}
		void AddMesh(Ref<VertexArray>& vertexarray)
		{
			vertexArray= vertexarray;
		}
		void Initialize(std::vector<std::pair<uint32_t, std::string>>& nodes)
		{
			for (auto& element : nodes)
			{
				const std::string& nodename = element.second;
				uint32_t id = element.first;
				auto iter = m_OffsetMatrix.find(nodename);
				if (iter != m_OffsetMatrix.end())
				{
					m_nodes.push_back(element);
				}
			}
		}
	};
	struct MaterialComponent
	{
		Material material;
		MaterialComponent() = default;
		MaterialComponent(Material& material)
		{
			this->material = material;
		}

		Material& GetMaterial() { return material; }
	};
	struct RelationShipComponent
	{
		uint32_t firstChild= 0xFFFFFFFF;
		uint32_t prevSibling= 0xFFFFFFFF;
		uint32_t nextSibling= 0xFFFFFFFF;
		uint32_t parent = 0xFFFFFFFF;
		RelationShipComponent()=default;
		bool dead = false;
	};
	struct LightComponent
	{
		enum class LightType
		{
			Directional=0,Point,Spot
		};
		LightComponent()
		{
			Validate();
		};
		void Validate()
		{
			Sampler_Desc desc;
			switch (type)
			{
			case LightType::Directional:
				if (debugMap != nullptr)
					debugMap.reset();
				resolution = 2048;
				debugMap = Texture2D::Create(TextureFormat::RGBA8, desc, resolution, resolution);
				break;
			case LightType::Spot:
				if (debugMap != nullptr)
					debugMap.reset();
				resolution = 512;
				debugMap = Texture2D::Create(TextureFormat::RGBA8, desc, 512, 512);
				far_plane = 10.0f;
				near_plane = 0.1f;
				break;
			case LightType::Point:
				if (debugMap != nullptr)
					debugMap.reset();
				resolution = 512;
				debugMap = Texture2D::Create(TextureFormat::RGBA8, desc, 512, 512);
				far_plane = 10.0f;
				near_plane = 0.1f;
				break;
			default:
				break;
			}
		}
		glm::vec3 diffuse = { 1.0f,1.0f,1.0f };
		glm::vec3 ambient = { 0.2f,0.2f,0.2f };
		glm::vec3 specular = { 1.0f,1.0f,1.0f };
		glm::vec3 lightDirection = { 0.0f,0.0f,1.0f };

		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
		float cutoff = 12.5f;
		float outerCutOff = 17.5f;
		float radius = 0.0f;
		float far_plane = 10.0f;
		float near_plane = 0.1f;
		uint32_t resolution = 1024;
		LightType type = LightType::Directional;

		//Ref<Texture> shadowMap;
		Ref<Texture> debugMap;
		int textureindex = 0;
		int lightindex = 0;
	};
	struct AnimatorComponent
	{
		Animator animator;
		AnimatorComponent() = default;
		AnimatorComponent(Animator& anim)
		{
			this->animator = anim;
		}
	};
}