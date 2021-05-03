#pragma once
#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace QCat
{
	class Renderer
	{
	public:
		static void Init(uint32_t width, uint32_t height);
		static void Shutdown();

		static void OnWindowResize(unsigned int width, unsigned int height);
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader,const unsigned int indexCount, const glm::mat4& transform = glm::mat4(1.0f));
		inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};
}