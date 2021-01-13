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

		// Render 2D sprites
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
			Renderer2D::BeginScene(*maincamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
			Renderer2D::EndScene();
		}
	}
	void Scene::OnViewportReSize(uint32_t width, uint32_t height)
	{
		m_ViewPortWidth = width;
		m_ViewPortHeight = height;

		// Resize our non-FixedAsepectRatio Camera
		auto& view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}
}