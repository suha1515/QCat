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
	uint32_t Entity::GetUID()
	{
		auto& uidComp = GetComponent<GuidComponent>();
		return uidComp.uid;
	}
	void Entity::SetParent(Entity* parent)
	{
		//RelationShip Component (for hierarchy Structure)
		RelationShipComponent& comp = this->GetComponent<RelationShipComponent>();
		uint32_t uid = GetUID();
		if (parent != nullptr)
		{
			comp.parent = parent->GetUID();

			RelationShipComponent& parentcomp = m_pScene->GetEntityById(comp.parent).GetComponent<RelationShipComponent>();
			auto curr = parentcomp.firstChild;
			auto curEntity = m_pScene->GetEntityById(curr);
			if (curEntity == Entity::emptyEntity)
			{
				parentcomp.firstChild = uid;
			}
			else
			{
				while (curEntity != Entity::emptyEntity)
				{
					RelationShipComponent& comp = curEntity.GetComponent < RelationShipComponent>();
					auto nextSibling = m_pScene->GetEntityById(comp.nextSibling);
					if (nextSibling == Entity::emptyEntity)
					{
						comp.nextSibling = uid;
						this->GetComponent<RelationShipComponent>().prevSibling = curEntity.GetUID();
						break;
					}
					curEntity = nextSibling;
				}
			}
		}
	}
}