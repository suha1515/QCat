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
		Ref<QCat::FrameBuffer> m_Framebuffer;
		Ref<QCat::Texture2D> m_Texture;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
	};
}
