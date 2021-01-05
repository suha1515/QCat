#include "qcpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <QCat/Renderer/RenderAPI.h>
namespace QCat
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		:m_ViewMatrix(1.0f),m_Position(glm::vec3(0.0f)),m_Rotation(0.0f)
	{
		QCAT_PROFILE_FUNCTION();

		RenderAPI::API api = RenderAPI::GetAPI();
		switch (api)
		{
		case RenderAPI::API::None:
			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected!");
			break;
		case RenderAPI::API::OpenGL:
		{
			m_ProjectionMatrix = glm::orthoLH(left, right, bottom, top, 0.0f, 1.0f);
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			break;
		}
		case RenderAPI::API::DirectX11:
		{
			m_ProjectionMatrix = glm::orthoLH(left, right, bottom, top, 0.0f, 1.0f);
			//DirectX::XMMATRIX mat;
			//mat = DirectX::XMMatrixOrthographicLH(right - left ,bottom - top,0.0f,1.0f);
			//mat =  DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, 0.0f, 1.0f);
			m_ViewProjectionMatrix =  m_ProjectionMatrix * m_ViewMatrix;
			break;
		}
		}
	}
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		QCAT_PROFILE_FUNCTION();

		RenderAPI::API api = RenderAPI::GetAPI();
		switch (api)
		{
		case RenderAPI::API::OpenGL: m_ProjectionMatrix = glm::orthoLH(left, right, bottom, top, 0.0f, 1.0f);  break;
		case RenderAPI::API::DirectX11: m_ProjectionMatrix = glm::orthoLH(left, right, bottom, top, 0.0f, 1.0f); break;
		}
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::RecalculateViewMatrix()
	{
		QCAT_PROFILE_FUNCTION();

		RenderAPI::API api  = RenderAPI::GetAPI();
		switch (api)
		{
		case RenderAPI::API::None:
			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected!");
			break;
		case RenderAPI::API::OpenGL:
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
				glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

			m_ViewMatrix = glm::inverse(transform);
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			break;
		}
		case RenderAPI::API::DirectX11:
		{
			glm::mat4 transform =  
				 glm::translate(glm::mat4(1.0f), m_Position)* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

			m_ViewMatrix = glm::inverse(transform);
			m_ViewProjectionMatrix =(m_ProjectionMatrix* m_ViewMatrix);
			break;
		}
			
		}
		
	}
}