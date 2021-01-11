#include "qcpch.h"
#include "Entity.h"

namespace QCat
{
	Entity::Entity(entt::entity handle, Scene* pScene)
		:m_EntityHandle(handle), m_pScene(pScene)
	{
	}
}