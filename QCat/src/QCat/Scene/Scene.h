#pragma once
#include "entt.hpp"

#include "QCat/Uitiliy/Timestep.h"

namespace QCat
{
	class Entity;
	class Scene
	{
		friend class Entity;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep ts);
		// Temporary
		entt::registry& Reg() { return m_Registry; }
	private:
		entt::registry m_Registry;		// container for all component (identifier)

	};
}