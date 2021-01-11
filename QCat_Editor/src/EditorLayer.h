#pragma once

#include "QCat.h"

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
		//camera
		OrthographicCameraController m_CameraController;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Texture2D> m_Texture;

		Ref<Scene> m_ActiveScene;
		entt::entity m_SquareEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
	};
}
