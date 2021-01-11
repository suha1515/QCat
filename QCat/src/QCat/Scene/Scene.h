#pragma once
#include "entt.hpp"
#include "Components.h"
#include "QCat/Uitiliy/Timestep.h"

namespace QCat
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		void OnUpdate(Timestep ts);
		// Temporary
		entt::registry& Reg() { return m_Registry; }
	private:
		entt::registry m_Registry;		// container for all component (identifier)

	};

}