#pragma once
#include <QCat/Scene/Scene.h>
#include <QCat/Core/Log.h>
#include <QCat/Scene/Entity.h>
#include <QCat/Core/QCatCore.h>

namespace QCat
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImguiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
		friend class Scene;
	};
}