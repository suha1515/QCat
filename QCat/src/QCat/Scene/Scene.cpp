#include "qcpch.h"
#include "Scene.h"
#include <glm/glm.hpp>

#include "QCat/Renderer/Renderer2D.h"

#include "Components.h"
#include "Entity.h"

namespace QCat
{
	static void DoMath(const glm::mat4& transform)
	{

	}
	Scene::Scene()
	{
		//-- Example --//
		//entt::entity entity = m_Registry.create(); //entity is uint32_t (actullay this is kind of identifier for entitiy)

		//// bellow code present how to use entt, registry(entt container) make entity using given struct or class Compoenent with extra argument( this is need to be match thoes struct)
		//m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		//// and emplace return component that is belong to entity
		////auto& transform = m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		//// by assigning through component , we can renew the value 
		////transform = glm::mat4(1.0f);

		//// and get method retrieve the component from entity
		//if(m_Registry.has<TransformComponent>(entity))
		//TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		//// view return all entity that has related with component
		//auto& view = m_Registry.view<TransformComponent>();
		//// this can be really good for performence
		//for (auto& entity : view)
		//{
		//	TransformComponent& transform = view.get<TransformComponent>(entity);
		//}

		//// for multipleComponent you can you group
		//auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		//for (auto& entity : group)
		//{
		//	auto&[transform,mesh]= group.get<TransformComponent, MeshComponent>(entity);
		//	
		//}
	}
	Scene::~Scene()
	{
	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity  entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "empty" : name;
	
		return  entity;
	}
	void Scene::OnUpdate(Timestep ts)
	{
		// for multipleComponent you can you group
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

		for (auto& entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}
}