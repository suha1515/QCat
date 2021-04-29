#include "qcpch.h"
#include "Entity.h"

namespace QCat
{
	Entity Entity::emptyEntity{};
	Entity::Entity(entt::entity handle, Scene* pScene)
		:m_EntityHandle(handle), m_pScene(pScene)
	{
	}
}