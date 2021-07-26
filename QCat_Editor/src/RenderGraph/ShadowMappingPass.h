#pragma once
#include "QCat.h"

namespace QCat
{

	class ShadowMappingPass : public Pass
	{
	public:
		struct LightMatrix
		{
			glm::mat4 matrices[3];
			glm::vec4 cascadedEndClip[3];
		};
	public:
		ShadowMappingPass(unsigned int level, const std::string& name);
		~ShadowMappingPass() = default;

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

		void DrawModel(entt::registry& registry);
		void CsmPass(glm::vec3 lightDirection, glm::vec3 upVector);
	private:

		Ref<Shader> m_PointshadowMappingShader;
		Ref<Shader> m_DirectionalshadowMappingShader;
		Ref<Shader> m_DebugShadowShader;
		Ref<SamplerState> m_SamplerForDebug;

		Ref<FrameBufferEx> m_PointLightShadow;
		Ref<FrameBufferEx> m_DirectionalLightShadow;
		Ref<FrameBufferEx> m_SpotLightShadow;
		Ref<FrameBufferEx> m_ColorBuffer;
		Ref<VertexArray> m_quad;

		glm::mat4 shadowProj;
		glm::mat4 captureViews[6];
		
		struct ShadowMatrix
		{
			glm::mat4 matrices[6];
		};
		struct CsmMatrix
		{
			glm::mat4 matrices[3];
			glm::mat4 projmatrices[3];
		};
		struct Transform
		{
			glm::mat4 transform;
			glm::mat4 invtrnasform;
		};
		struct DirectionalLightViewProj
		{
			glm::mat4 viewProjMatrix;
			float nearz;
			float farz;
		};
		
		Ref<ConstantBuffer> transformConstantBuffer;
		Ref<ConstantBuffer> shadowMapMatrices;
		Ref<ConstantBuffer> csmMatrices;
		Ref<ConstantBuffer> directionallightMatrix;

		Ref<glm::mat4> viewMatrix;
		Ref<glm::mat4> projectionMatrix;
		Ref<glm::vec4> forArNearFar;
		Ref<LightMatrix> DirlightTransform;

		glm::mat4 m_shadowOrthoProj[3];

		float m_cascadeEnd[4];

	};
}