#pragma once
#include "QCat.h"

namespace QCat
{
	class PostProcessPass : public Pass
	{
	public:
		PostProcessPass(unsigned int level, const std::string& name);
		~PostProcessPass();

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

		Ref<Texture>& GetColorBuffer() { return m_ColorBuffer; }
	private:
		//FrameBuffer
		Ref<FrameBufferEx> m_ScreenFrameBuffer;
		//Shader
		Ref<Shader> m_ScreenShader;
		Ref<Shader> m_OutlineShader;
		Ref<Shader> m_VerticalBlurShader;
		Ref<Shader> m_HorizontalBlurShader;
		//Input
		Ref<Texture> m_ColorBuffer;
		Ref<Texture> m_DepthBuffer;


		Ref<Texture> m_ScreenTexture;

		struct Color
		{
			glm::vec4 color;
		};
		struct Outline
		{
			glm::vec4 color;
			float outlineThickness;
			float outlineThreshHold;
		};
		Ref<ConstantBuffer> m_ColorConstantBuffer;
		Ref<ConstantBuffer> m_OutlineConstantBuffer;
	};
}