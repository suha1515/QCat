#include "qcpch.h"
#include "Scene.h"
#include <glm/glm.hpp>

#include "QCat/Renderer/Renderer2D.h"

#include "Components.h"
#include "Entity.h"

namespace QCat
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	Entity Scene::CreateEntity(const std::string& name , uint32_t id)
	{
		m_Registry.create();
		Entity  entity = { m_Registry.create(), this};
		auto& uidcomp = entity.AddComponent<GuidComponent>();
		entity.AddComponent<TransformComponent>();
		RelationShipComponent& comp = entity.AddComponent<RelationShipComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "empty" : name;

		if (id != 0)
		{
			uidcomp.uid = id;
			if (m_entityMap.find(id) == m_entityMap.end())
				m_entityMap.insert({ id, entity });
			else
				QCAT_CORE_ASSERT(false, "CreateEntityError : Entity UID isnt unisque!");
		}
		else
		{
			uidcomp.uid  = static_cast<std::underlying_type_t< entt::entity>>(entity.GetHandle());
			if (m_entityMap.find(entity.GetUID()) == m_entityMap.end())
				m_entityMap.insert({ entity.GetUID(), entity });
			else
				QCAT_CORE_ASSERT(false, "CreateEntityError : Entity UID isnt unisque!");
		}
		return  entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		RelationShipComponent& comp = entity.GetComponent<RelationShipComponent>();
		comp.dead = true;
		auto parent = m_entityMap.find(comp.parent);
		if (parent != m_entityMap.end())
		{
			RelationShipComponent &parentcomp = parent->second.GetComponent<RelationShipComponent>();
			if (parentcomp.dead == false)
			{
				if (entity.GetUID() == parentcomp.firstChild)
				{
					auto nexSibling = m_entityMap.find(comp.nextSibling);
					if (nexSibling != m_entityMap.end())
						parentcomp.firstChild = comp.nextSibling;
					else
						parentcomp.firstChild = 0xFFFFFFFF;
				}
				else
				{	
					auto prevSibling = m_entityMap.find(comp.prevSibling);
					auto nextSibling = m_entityMap.find(comp.nextSibling);
					if (nextSibling != m_entityMap.end())
					{
						prevSibling->second.GetComponent<RelationShipComponent>().nextSibling = nextSibling->first;
						nextSibling->second.GetComponent<RelationShipComponent>().prevSibling = prevSibling->first;
					}
					else
						prevSibling->second.GetComponent<RelationShipComponent>().nextSibling = 0xFFFFFFFF;
					
				}
			}
		}		
		auto child = m_entityMap.find(comp.firstChild);
		while (child != m_entityMap.end())
		{
			auto nextSibling = child->second.GetComponent<RelationShipComponent>().nextSibling;
			DestroyEntity(child->second);
			child = m_entityMap.find(nextSibling);
		}
		auto iter = m_entityMap.find(entity.GetUID());
		if(iter!=m_entityMap.end())
			m_entityMap.erase(iter);
		m_Registry.destroy(entity);

	}
	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		
	}
	void Scene::UpdateRecursively(Entity& entity,const glm::mat4& parentMatrix)
	{	
			auto& rc = entity.GetComponent<RelationShipComponent>();
			auto childid = rc.firstChild;
			while (childid != 0xFFFFFFFF)
			{
				auto& childEntity = m_entityMap[childid];
				auto& ctc = childEntity.GetComponent<TransformComponent>();
				auto& crc = childEntity.GetComponent<RelationShipComponent>();
				ctc.parentMatrix = parentMatrix;
				ctc.changed = false;
				if (crc.firstChild != 0xFFFFFFFF)
				{
					UpdateRecursively(childEntity,ctc.GetTransform());
				}
				childid = crc.nextSibling;
			}
	}
	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Update Script
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					// TODO : Move to Scene::OnScenePlay (Engine Play button)
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity,this };
						nsc.Instance->OnCreate();
					}
					nsc.Instance->OnUpdate(ts);
				}
			);
		}
		// Update Entity
		for (auto& entity : m_entityMap)
		{
			auto& tc = entity.second.GetComponent<TransformComponent>();
			if (tc.changed)
			{
				UpdateRecursively(entity.second,tc.GetTransform());
				tc.changed = false;
			}
		}
		// Render
		Camera* maincamera = nullptr;
		glm::mat4 cameraTransform ;
		{
			auto view = m_Registry.view<TransformComponent,CameraComponent>();
			for (auto& entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary)
				{
					maincamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		if (maincamera)
		{
		}
	}
	void Scene::OnViewportReSize(uint32_t width, uint32_t height)
	{
		m_ViewPortWidth = width;
		m_ViewPortHeight = height;

		// Resize our non-FixedAsepectRatio Camera
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}
	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity,this };
		}
		return {};
	}

	Entity Scene::GetEntityById(uint32_t id)
	{
		auto iter = m_entityMap.find(id);
		if (iter != m_entityMap.end())
			return iter->second;
		else
			return Entity::emptyEntity;
	}

	void Scene::UpdateEntity()
	{

	}
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);

	}
	template<>
	void Scene::OnComponentAdded<GuidComponent>(Entity entity, GuidComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewPortWidth, m_ViewPortHeight);
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<RelationShipComponent>(Entity entity, RelationShipComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<AnimatorComponent>(Entity entity,AnimatorComponent& component)
	{}
	
}