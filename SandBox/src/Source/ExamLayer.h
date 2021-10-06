#pragma once
#include <QCat.h>

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
	};
}
