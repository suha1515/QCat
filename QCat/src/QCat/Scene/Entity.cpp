#include "qcpch.h"
#include "Entity.h"
#include "Components.h"
namespace QCat
{
	Entity Entity::emptyEntity{};
	Entity::Entity(entt::entity handle, Scene* pScene)
		:m_EntityHandle(handle), m_pScene(pScene)
	{
	}
	void Entity::SetParent(Entity* parent)
	{
		//RelationShip Component (for hierarchy Structure)
		RelationShipComponent& comp = this->GetComponent<RelationShipComponent>();
		
		if (parent != nullptr)
		{
			comp.parent = parent->GetUID();
			
			RelationShipComponent& parentcomp = m_pScene->GetEntityById(comp.parent).GetComponent<RelationShipComponent>();
			auto curr = parentcomp.firstChild;
			auto curEntity = m_pScene->GetEntityById(curr);
			if (curEntity == Entity::emptyEntity)
			{
				parentcomp.firstChild = m_uid;
			}
			else
			{
				while (curEntity !=Entity::emptyEntity)
				{
					RelationShipComponent& comp = curEntity.GetComponent < RelationShipComponent>();
					auto nextSibling = m_pScene->GetEntityById(comp.nextSibling);
					if (nextSibling == Entity::emptyEntity)
					{
						comp.nextSibling = m_uid;
						this->GetComponent<RelationShipComponent>().prevSibling = nextSibling.m_uid;
						break;
					}
					curEntity = m_pScene->GetEntityById(comp.nextSibling);
				}
			}
		}
	}
}