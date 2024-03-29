#pragma once
#include <QCat.h>
#include "../Geometry/Cube.h"
namespace QCat
{
	class ExamLayer : public Layer
	{
	public:
		struct Data
		{
			glm::vec3 v1;
			glm::vec2 v2;
		};
		ExamLayer()
			:Layer("Example")
		{

		}
		void OnAttach() override;
		void OnUpdate(Timestep step) override;
		virtual void OnImGuiRender() override
		{
			//ImGui::ShowDemoWindow();

		}
		void OnEvent(Event& e) override
		{
			//m_CameraController.OnEvent(e);
		}

	private:
		Ref<VertexArray> dynamicBuffer;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> IndexBuffer;

		Ref<Shader> particleShader;

		struct modelInfo
		{
			glm::mat4 transform;
			glm::mat4 Invtransform;
		};

		struct cameraInfo
		{
			glm::mat4 projection;
			glm::mat4 view;
			glm::vec3 viewPos;
		};
		Ref<ConstantBuffer> transformBuffer;
		Ref<ConstantBuffer> cameraBuffer;
		Ref<Cube> cube;


		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::vec3 cameraPosition;
	};
}
