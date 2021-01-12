#include "qcpch.h"
#include "OrthographicCameraController.h"

#include "QCat/Core/Input.h"


namespace QCat
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio,bool rotation)
		:m_aspectRatio(aspectRatio),m_Camera(-m_aspectRatio*m_zoomLevel, m_aspectRatio * m_zoomLevel,
			-m_zoomLevel,m_zoomLevel),m_rotation(rotation)
	{
	}
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(Key::A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		else if (QCat::Input::IsKeyPressed(Key::D))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;

		if (Input::IsKeyPressed(Key::W))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(Key::S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;

		if (m_rotation)
		{
			if (Input::IsKeyPressed(Key::Q))
				m_CameraRoatation += m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::E))
				m_CameraRoatation -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRoatation);
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_CameraTranslationSpeed = m_zoomLevel;
	}
	void OrthographicCameraController::OnEvent(Event& e)
	{
		QCAT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}
	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_aspectRatio = width / height;
		m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}
	bool OrthographicCameraController::OnMouseScrolled(MouseScrollEvent& e)
	{
		QCAT_PROFILE_FUNCTION();

		m_zoomLevel -=e.GetOffsetY()*0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel,-m_zoomLevel, m_zoomLevel);
		return false;
	}
	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth(),(float)e.GetHeight());
		return false;
	}
}