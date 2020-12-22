#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Uitiliy/Timestep.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

namespace QCat
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation=false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrolled(MouseScrollEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_aspectRatio;
		float m_zoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_rotation = false;
		glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
		float m_CameraRoatation = 0.0f;

		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};
}