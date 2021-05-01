#pragma once

#include "QCat.h"
#include "Pannels/SceneHierarchyPanel.h"
#include <QCat/Renderer/EditorCamera.h>

namespace QCat
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(QCat::Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(QCat::Event& e) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		//camera
		OrthographicCameraController m_CameraController;
		Ref<FrameBufferEx> m_FrameBufferEx;
		Ref<Texture2D> m_Texture;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;
		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;
		// Panels	
		SceneHierarchyPanel m_SceneHierarchyPanel;

	};
}
