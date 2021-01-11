#include "qcpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace QCat
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthoGraphic(float size, float nearClip, float farClip)
	{
		m_OrthoGraphicSize = size;
		m_OrthoGraphicNear = nearClip;
		m_OrthoGraphicfar = farClip;

		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{
		float orthoLeft = -m_OrthoGraphicSize * 0.5f * m_AspectRatio;
		float orthoRight = +m_OrthoGraphicSize * 0.5f * m_AspectRatio;
		float orthoBottom = -m_OrthoGraphicSize * 0.5f;
		float orthoTop = m_OrthoGraphicSize * 0.5f;

		m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,
			m_OrthoGraphicNear, m_OrthoGraphicfar);
	}
}