#pragma once
#include "QCat.h"
#include "../Geometry/Sphere.h"
#include "../Geometry/Cube.h"
#include "ShadowMappingPass.h"

namespace QCat
{
	class PBRShaderPass : public Pass
	{
	public:
		PBRShaderPass(unsigned int level, const std::string& name);
		~PBRShaderPass() = default;

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

	private:
		Ref<Texture> m_HdrCubeMap;
		Ref<Texture> m_IrradianceCubeMap;
		Ref<Texture> m_BRDFLutTextrue;
		Ref<Texture> m_PrefilterMap;

		Ref<Shader>  m_PBRShader;
		Ref<Shader>  m_FlatColorShader;
		Ref<Shader>  m_SkyBoxShader;
		
		Ref<glm::mat4> viewMatrix;
		Ref<glm::mat4> projectionMatrix;

		Ref<FrameBufferEx> m_Framebuffer;

		Ref<Texture> m_ColorBuffer;
		Ref<Texture> m_DepthBuffer;
		Ref<ShadowMappingPass::LightMatrix> dirlightTransform;
		Ref<ConstantBuffer> dirlighbuffer;
		Ref<Texture> m_DirLightMap;
		//Material materials[5];
		Ref<Cube> cube;
		Ref<Sphere> sphere;

		struct CameraData
		{
			glm::mat4 projection;
			glm::mat4 view;
			glm::vec3 position;
		};
		struct Transform
		{
			glm::mat4 transform;
			glm::mat4 invtrnasform;
		};
		struct Mat
		{
			glm::vec3 albedo;
			float shininess;
			float metallic;
			float roughness;
			float ambientocclusion;

			int IsAlbedoMap;
			int IsNormalMap;
			int IsMetallicMap;
			int IsRoughnessMap;
			int IsAoMap;
		};
		struct DirLight
		{
			glm::vec3 lightDirection = { 0.0f,0.0f,0.0f };
			float padding;
			glm::vec3 diffuse = { 0.0f,0.0f,0.0f };
			float isActive=0.0f;
			int isDebug = 0;
			int isSoft = 0;
		};
		struct PointLight
		{
			glm::vec3 position = { 0.0f,0.0f,0.0f };
			float padding;
			glm::vec3 diffuse = { 0.0f,0.0f,0.0f };
			float isActive =0.0f;
		};
		struct Light
		{
			PointLight pointLight[4];
			DirLight dirlight;
		};
	
		struct color
		{
			glm::vec4 color;
		};
		CameraData camData;
		Transform transformData;
		Mat		matData;

		color colData;
		Ref<ConstantBuffer> cameraConstantBuffer;
		Ref<ConstantBuffer> transformConstantBuffer;
		Ref<ConstantBuffer> materialConstantBuffer;
		Ref<ConstantBuffer> lightConstantBuffer;
		Ref<ConstantBuffer> colorConstantBuffer;

		Ref<ElementBuffer> transformBuffer;
		Ref<bool> m_SoftShadow;
		Ref<bool> m_DebugShadow;

		Ref<SamplerState> m_normalSampler;

	};
}