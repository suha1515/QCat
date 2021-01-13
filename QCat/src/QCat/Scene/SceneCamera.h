#pragma once

#include "QCat/Renderer/Camera.h"

namespace QCat
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType{Perspective=0,Orthographic=1};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthoGraphic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFov, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFov()     const { return m_PerspectiveFov; }
		float GetPerspectiveNearClip()		  const { return m_PerspectiveNear; }
		float GetPerspectiveFarClip()		  const { return m_PerspectiveFar; }

		void  SetPerspectiveVerticalFov(float fov)   { m_PerspectiveFov = fov; RecalculateProjection(); }
		void  SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		void  SetPerspectiveFarClip(float farClip)   { m_PerspectiveFar = farClip; RecalculateProjection(); }

		float GetOrthoGraphicSize()			 const { return m_OrthoGraphicSize; }
		float GetOrthographicNearClip()		 const { return m_OrthoGraphicNear; }
		float GetOrthographicFarClip()		 const { return m_OrthoGraphicfar; }

		void  SetOrthoGraphicSize(float size)        { m_OrthoGraphicSize = size; RecalculateProjection(); }
		void  SetOrthographicNearClip(float nearClip){ m_OrthoGraphicNear = nearClip; RecalculateProjection();}
		void  SetOrthographicFarClip(float farClip)  { m_OrthoGraphicfar  = farClip ; RecalculateProjection();}

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection();	}
	private:
		void RecalculateProjection();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFov = glm::radians(45.0f);
		float m_PerspectiveNear = 0.001f, m_PerspectiveFar = 1000.0f;

		float m_OrthoGraphicSize = 10.0f;
		float m_OrthoGraphicNear = 0.0f, m_OrthoGraphicfar  = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}
