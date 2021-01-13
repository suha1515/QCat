#pragma once
#include "entt.hpp"
#include "QCat/Uitiliy/Timestep.h"

namespace QCat
{
	class Entity;
	class Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep ts);
		void OnViewportReSize(uint32_t width, uint32_t height);
		// Temporary
		entt::registry& Reg() { return m_Registry; }
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;		// container for all component (identifier)
		uint32_t m_ViewPortWidth = 0, m_ViewPortHeight = 0;
	};
}