#pragma once
#include "entt.hpp"
#include "QCat/Uitiliy/Timestep.h"
#include "QCat/Renderer/EditorCamera.h"

namespace QCat
{
	class Entity;
	class Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string(), uint32_t id = 0);
		void DestroyEntity(Entity entity);

		void OnUpdateEditor(Timestep ts,EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);
		void OnViewportReSize(uint32_t width, uint32_t height);
		
		Entity GetPrimaryCameraEntity();
		entt::registry& GetRegistry() {return m_Registry;}

		Entity GetEntityById(uint32_t id);

		void UpdateEntity();
		void UpdateRecursively(Entity& entity, const glm::mat4& parentMatrix);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;		// container for all component (identifier)
		uint32_t m_ViewPortWidth = 0, m_ViewPortHeight = 0;

		std::map<uint32_t, Entity> m_entityMap;
	};
}