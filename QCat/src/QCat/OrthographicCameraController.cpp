#include "qcpch.h"
#include "OrthographicCameraController.h"

#include "Input.h"


namespace QCat
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio,bool rotation)
		:m_aspectRatio(aspectRatio),m_Camera(-m_aspectRatio*m_zoomLevel, m_aspectRatio * m_zoomLevel,
			-m_zoomLevel,m_zoomLevel),m_rotation(rotation)
	{
	}
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(QCAT_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		else if (QCat::Input::IsKeyPressed(QCAT_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;

		if (Input::IsKeyPressed(QCAT_KEY_W))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(QCAT_KEY_S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;

		if (m_rotation)
		{
			if (Input::IsKeyPressed(QCAT_KEY_Q))
				m_CameraRoatation += m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(QCAT_KEY_E))
				m_CameraRoatation -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRoatation);
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_CameraTranslationSpeed = m_zoomLevel;
	}
	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}
	bool OrthographicCameraController::OnMouseScrolled(MouseScrollEvent& e)
	{
		m_zoomLevel -=e.GetOffsetY()*0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel,-m_zoomLevel, m_zoomLevel);
		return false;
	}
	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_aspectRatio = (float)e.GetWidth()/(float)e.GetHeight();
		m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}
}