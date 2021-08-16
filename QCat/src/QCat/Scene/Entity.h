#pragma once
#include "Scene.h"
#include "entt.hpp"
#include "QCat/Core/Assert.h"

namespace QCat
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* pscene);
		Entity(const Entity& other) = default;

		uint32_t GetUID();
		void SetParent(Entity* parent);

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			QCAT_CORE_ASSERT(!HasComponent<T>(),"Entity already has Component!");
			T& component = m_pScene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_pScene->OnComponentAdded<T>(*this,component);
			return component;
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
		operator entt::entity()const { return m_EntityHandle; }
		operator uint32_t()const { return (uint32_t)m_EntityHandle; }
		
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_pScene == other.m_pScene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
		bool operator == (const entt::entity& other) const { return m_EntityHandle == other; }
		bool operator!=(const entt::entity& other) const { return !(m_EntityHandle == other); }
		Entity& operator =(const entt::entity& other) { m_EntityHandle = other; return *this; }
		entt::entity GetHandle() { return m_EntityHandle; }
		Scene* GetScene() { return m_pScene; };
		//void SelectEntity();
	public:
		static Entity emptyEntity;
		bool isSelected = false;
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_pScene=nullptr;	
	};
}

