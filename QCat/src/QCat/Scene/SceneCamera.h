#pragma once

#include "QCat/Renderer/Camera.h"

namespace QCat
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthoGraphic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthoGraphicSize() const { return m_OrthoGraphicSize; }
		void  SetOrthoGraphicSize(float size) { m_OrthoGraphicSize = size; RecalculateProjection();}
	private:
		void RecalculateProjection();
	private:
		float m_OrthoGraphicSize = 10.0f;
		float m_OrthoGraphicNear = 0.0f, m_OrthoGraphicfar  = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}
