#pragma once
#include"QCat.h"
#include "../../Geometry/Cube.h"

namespace QCat
{
	class PBRPreComputePass :public Pass
	{
	public:
		PBRPreComputePass(unsigned int level, const std::string& name);
		~PBRPreComputePass() = default;

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

		void MakeTextureMap();

	private:
		bool isHdrImageChange = true;
		Ref<Texture> m_HdrImage;
		Ref<Texture> m_IrradianceCubeMap;
		Ref<Texture> m_HdrCubeMap;
		Ref<Texture> m_BRDFLutTexture;
		Ref<Texture> m_PrefilterMap;

		Ref<Shader> m_BRDFLutShader;
		Ref<Shader> m_HdrToCubeMapShader;
		Ref<Shader> m_HdrCubeMapShader;
		Ref<Shader> m_IrradianceMapShader;
		Ref<Shader> m_PrefilterShader;

		Ref<VertexArray>m_quad;

		glm::mat4 captureProjection;
		glm::mat4 captureViews[6];

		Ref<FrameBufferEx> CubeMapPass;

		Ref<Cube> cube;
		struct CameraData
		{
			glm::mat4 projection;
			glm::mat4 view;
		};
		struct Roughness
		{
			float roughtness;
		};
		CameraData CameraBuffer;
		Roughness roughBuffer;
		Ref<ConstantBuffer> cameraConstantBuffer;
		Ref<ConstantBuffer> roughnessConstantBuffer;
	};
}
