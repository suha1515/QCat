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
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string & tag)
			:Tag(tag) {}
	};
	struct TransformComponent
	{
		glm::mat4 parentMatrix = glm::mat4(1.0f);
		glm::vec3 Translation = { 0.0f,0.0f,0.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			:Translation(translation) {}
		void SetTransform(const glm::mat4& transform)
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			Translation = translation;
			Rotation = rotation;
			Scale = scale;
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
	struct MeshComponent
	{
		std::vector<Ref<VertexArray>> vertexArray;
		MeshComponent() = default;
		MeshComponent(const std::string& meshName)
		{
			vertexArray.push_back(MeshLibrary::Load(meshName));
		}
		MeshComponent(const std::string& meshName,Ref<VertexArray>& vertexarray)
		{
			MeshLibrary::Set(meshName, vertexarray);
			vertexArray.push_back(std::move(vertexarray));
		}
		/*void Bind()
		{
			vertexArray->Bind();
		}
		void UnBind()
		{
			vertexArray->UnBind();
		}*/
		std::vector<Ref<VertexArray>>& GetMeshes() { return vertexArray; }
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
		Entity firstChild;
		Entity prevSibling;
		Entity nextSibling;
		Entity parent;
		RelationShipComponent()=default;
		bool dead = false;
	};
	struct LightComponent
	{
		enum class LightType
		{
			Directional=0,Point,Spot
		};
		LightComponent() = default;
		glm::vec3 diffuse = { 1.0f,1.0f,1.0f };
		glm::vec3 ambient = { 0.2f,0.2f,0.2f };
		glm::vec3 specular = { 1.0f,1.0f,1.0f };
		glm::vec3 lightDirection;

		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
		float cutoff = glm::cos(glm::radians(12.5f));
		float outerCutOff = glm::cos(glm::radians(17.5f));
		float radius = 0.0f;
		LightType type = LightType::Directional;
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