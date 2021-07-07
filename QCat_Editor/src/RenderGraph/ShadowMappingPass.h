#pragma once
#include "QCat.h"

namespace QCat
{
	class ShadowMappingPass : public Pass
	{
	public:
		ShadowMappingPass(unsigned int level, const std::string& name);
		~ShadowMappingPass() = default;

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

		void DrawModel(entt::registry& registry);
	private:

		Ref<Shader> m_PointshadowMappingShader;
		Ref<Shader> m_DirectionalshadowMappingShader;
		Ref<Shader> m_DebugShadowShader;
		Ref<SamplerState> m_SamplerForDebug;

		Ref<FrameBufferEx> m_PointLightShadow;
		Ref<FrameBufferEx> m_DirectionalLightShadow;
		Ref<FrameBufferEx> m_ColorBuffer;
		Ref<VertexArray> m_quad;

		glm::mat4 shadowProj;
		glm::mat4 captureViews[6];
		
		struct ShadowMatrix
		{
			glm::mat4 matrices[6];
		};
		struct Transform
		{
			glm::mat4 transform;
			glm::mat4 invtrnasform;
		};
		struct DirectionalLightViewProj
		{
			glm::mat4 viewProjMatrix;
		};
		Ref<ConstantBuffer> transformConstantBuffer;
		Ref<ConstantBuffer> shadowMapMatrices;
		Ref<ConstantBuffer> directionallightMatrix;
	};
}