#pragma once
#include "QCat.h"

namespace QCat
{

	class ShadowMappingPass : public Pass
	{
	public:
		struct LightMatrix
		{
			glm::mat4 dirlightmatrices[5];
			glm::mat4 spotlightmatrices[8];
			glm::vec4 cascadedEndClip[6];
		};
	public:
		ShadowMappingPass(unsigned int level, const std::string& name);
		~ShadowMappingPass() = default;

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

		void DrawModel(entt::registry& registry);
		void CsmPass(glm::vec3 lightDirection, glm::vec3 upVector,float resoultion);
	private:

		Ref<Shader> m_PointshadowMappingShader;
		Ref<Shader> m_DirectionalshadowMappingShader;
		Ref<Shader> m_SpotLightShadowMappingShader;
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
			glm::mat4 matrices[5];
		};
		struct SpotMatrix
		{
			glm::mat4 matrix;
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
		Ref<ConstantBuffer> spotlightMatrix;

		Ref<glm::mat4> viewMatrix;
		Ref<glm::mat4> projectionMatrix;
		Ref<glm::vec4> forArNearFar;
		Ref<float> m_CascadeSplits;

		Ref<LightMatrix> DirlightTransform;

		glm::mat4 m_shadowOrthoProj[5];

		float m_cascadeEnd[6];

		Ref<Texture> m_DirLightShadowMap;
		Ref<Texture> m_PointLightShadowMap;
		Ref<Texture> m_SpotLightShadowMap;

	};
}