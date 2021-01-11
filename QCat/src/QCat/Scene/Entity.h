#pragma once
#include "Scene.h"
#include "entt.hpp"

namespace QCat
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle,Scene* pscene);
		Entity(const Entity& other) = default;

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			QCAT_CORE_ASSERT(!HasComponent<T>(),"Entity already has Component!");
			return m_pScene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}
		template<typename T>
		T& GetComponent()
		{
			QCAT_CORE_ASSERT(HasComponent<T>(), "Entity does not have Component!");
			return m_pScene->m_Registry.get<T>(m_EntityHandle);
		}
		template<typename T>
		bool HasComponent()
		{
			return m_pScene->m_Registry.has<T>(m_EntityHandle);
		}
		template<typename T>
		void RemoveComponent()
		{
			QCAT_CORE_ASSERT(HasComponent<T>(), "Entity does not have Component!");
			m_pScene->m_Registry.remove<T>(m_EntityHandle);
		}
		operator bool() const { return m_EntityHandle != entt::null; }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_pScene=nullptr;
	};
}

