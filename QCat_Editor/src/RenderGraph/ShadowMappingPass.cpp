#include "ShadowMappingPass.h"
#include <limits>

namespace QCat
{
	ShadowMappingPass::ShadowMappingPass(unsigned int level, const std::string& name)
	{
		this->level = level;
		this->name = name;

		RegisterInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
		RegisterInput(DataInput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));
		RegisterInput(DataInput<glm::vec4>::Make("forArNearFar", forArNearFar, DataType::Float4));
		RegisterInput(DataInput<float>::Make("CasacadeSplits", m_CascadeSplits, DataType::Float));

		// Directional Light ShadowTransform
		RegisterOutput(DataOutput<LightMatrix>::Make("DirlightTransform", DirlightTransform, DataType::Struct));
		// Directional Light ShadowMap
		RegisterOutput(TextureOutput::Make("DirectionalLightShadowMap", m_DirLightShadowMap));
		// Point Light ShadowMap
		RegisterOutput(TextureOutput::Make("PointLightShadowMap", m_PointLightShadowMap));
		// Spot Light ShadowMap
		RegisterOutput(TextureOutput::Make("SpotLightShadowMap", m_SpotLightShadowMap));

		DirlightTransform = CreateRef<LightMatrix>();
		Sampler_Desc desc;
		desc.MIN = Filtering::LINEAR;
		desc.MAG = Filtering::LINEAR;
		desc.MIP = Filtering::NONE;
		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.addressW = WrapingMode::CLAMP;
		desc.mode = FilterMode::COMPARISON;
		desc.comparisonFunc = COMPARISON_FUNC::LESS_EQUAL;
		desc.borderColor[0] = 1.0f;
		desc.borderColor[1] = 1.0f;
		desc.borderColor[2] = 1.0f;
		desc.borderColor[3] = 1.0f;
		m_DirLightShadowMap = Texture2DArray::Create(TextureFormat::DEPTH32, desc, 2048, 2048, 5);
		m_SpotLightShadowMap = Texture2DArray::Create(TextureFormat::DEPTH32, desc, 512, 512, 8);

		m_PointLightShadowMap = TextureCubeArray::Create(TextureFormat::DEPTH32, desc, 512, 512, 4);

		AttachmentSpecification pointdepthbuffer = { {FramebufferUsage::Depth,TextureType::TextureCube,TextureFormat::DEPTH32,"DepthBuffer"} };
		pointdepthbuffer.Height = 1024;
		pointdepthbuffer.Width = 1024;

		AttachmentSpecification directiondepthbuffer = { {FramebufferUsage::Depth,TextureType::Texture2DArray,TextureFormat::DEPTH32,"DepthBuffer"} };
		directiondepthbuffer.Height = 1024;
		directiondepthbuffer.Width = 1024;

		AttachmentSpecification spotLightDepthBuffer = { {FramebufferUsage::Depth,TextureType::Texture2DArray,TextureFormat::DEPTH32,"DepthBuffer"} };
		directiondepthbuffer.Height = 1024;
		directiondepthbuffer.Width = 1024;

		AttachmentSpecification colorBuffer = { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer"} };
		colorBuffer.Height = 1024;
		colorBuffer.Width = 1024;

		m_PointLightShadow = FrameBufferEx::Create(pointdepthbuffer);
		m_DirectionalLightShadow = FrameBufferEx::Create(directiondepthbuffer);
		m_SpotLightShadow = FrameBufferEx::Create(spotLightDepthBuffer);
		m_ColorBuffer = FrameBufferEx::Create(colorBuffer);

		shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);


		shadowMapMatrices = ConstantBuffer::Create(sizeof(ShadowMatrix), 0);
		csmMatrices = ConstantBuffer::Create(sizeof(CsmMatrix), 0);
		transformConstantBuffer = ConstantBuffer::Create(sizeof(Transform), 0);
		spotlightMatrix = ConstantBuffer::Create(sizeof(SpotMatrix), 0);

		Sampler_Desc desc2;
		m_SamplerForDebug = SamplerState::Create(desc2);
	}
	void ShadowMappingPass::Initialize()
	{
		m_PointshadowMappingShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? "Asset/shaders/glsl/ShadowMap/PointShadowMap.glsl" : "Asset/shaders/hlsl/ShadowMap/PointShadowMap.hlsl");
		m_DirectionalshadowMappingShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? "Asset/shaders/glsl/ShadowMap/DirectionalShadowMap.glsl" : "Asset/shaders/hlsl/ShadowMap/DirectionalShadowMap.hlsl");
		m_SpotLightShadowMappingShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? "Asset/shaders/glsl/ShadowMap/SpotLightShadowMap.glsl" : "Asset/shaders/hlsl/ShadowMap/SpotLightShadowMap.hlsl");
		m_DebugShadowShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? "Asset/shaders/glsl/ShadowMap/DebugShadow.glsl" : "Asset/shaders/hlsl/ShadowMap/DebugShadow.hlsl");

		float quadVertices[] =
		{
			-1.0f, 1.0f,  0.0f,0.0f,
			-1.0f,-1.0f,  0.0f,1.0f,
			 1.0f,-1.0f,  1.0f,1.0f,
			 1.0f, 1.0f,  1.0f,0.0f
		};

		Ref<VertexBuffer> quadBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));

		quadBuffer->SetLayout(BufferLayout::Create({
			{ShaderDataType::Float2,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoords"} }, m_DebugShadowShader
			));

		unsigned int indices[] =
		{
			1,0,3,1,3,2
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 6);
		m_quad = VertexArray::Create();
		m_quad->AddVertexBuffer(quadBuffer);
		m_quad->SetIndexBuffer(indexBuffer);
		m_quad->UnBind();
	}
	void ShadowMappingPass::DrawModel(entt::registry& registry)
	{
		auto view = registry.view<TransformComponent, MeshComponent>();
		for (auto& model : view)
		{
			auto& transformcomp = view.get<TransformComponent>(model);
			auto& meshcomp = view.get<MeshComponent>(model);
			if (meshcomp.GetMesh() != nullptr)
			{
				glm::mat4 transform = transformcomp.GetTransform();
				Transform buffer;
				buffer.transform = transform;
				buffer.invtrnasform = glm::inverse(transform);
				transformConstantBuffer->SetData(&buffer, sizeof(Transform), 0);

				RenderCommand::DrawIndexed(meshcomp.GetMesh());
			}
		}
	}
	std::vector<float> GetPracticalSplit(float nearz,float farz,int amount,float lamda)
	{
		std::vector<float> zSplits;
		zSplits.resize(amount);
		zSplits[0] = nearz;
		zSplits[amount - 1] = farz;
		for (int i = 1; i < amount-1; ++i)
		{
			float index = (i / (float)amount);
			float uniformSplit = nearz + (farz - nearz) * index;
			float logarithmSplit = nearz * std::powf((farz / nearz), index);
			zSplits[i] = Utils::Lerp(logarithmSplit, uniformSplit, lamda);
			
		}
		return zSplits;
	}
	void ShadowMappingPass::CsmPass(glm::vec3 lightDirection, glm::vec3 upVector,float resoultion)
	{
		// Get CameraInv Matrix
		glm::mat4 camview = (*viewMatrix);
		glm::mat4 projView = (*projectionMatrix) * camview;

		glm::mat4 camInv = glm::inverse(camview);
		float fov = forArNearFar->x;
		float ar = forArNearFar->y;
		float nearZ = forArNearFar->z;
		float farZ = forArNearFar->w;

		float tanHalfVFov = tanf(glm::radians(fov / 2.0f));
		float tanHalfHFov = tanHalfVFov * ar;

		std::vector<float> splits = GetPracticalSplit(nearZ, farZ, 6, *m_CascadeSplits);
		for (int i = 0 ; i < splits.size(); ++i)
		{
			m_cascadeEnd[i] = splits[i];
		}

		for (uint32_t i = 0; i < 5; ++i)
		{
			float xn = m_cascadeEnd[i] * tanHalfHFov;
			float xf = m_cascadeEnd[i + 1] * tanHalfHFov;
			float yn = m_cascadeEnd[i] * tanHalfVFov;
			float yf = m_cascadeEnd[i + 1] * tanHalfVFov;

			glm::vec4 frustumCorners[8] =
			{
				//near Face
				{xn,yn,m_cascadeEnd[i],1.0f},
				{-xn,yn,m_cascadeEnd[i],1.0f},
				{xn,-yn,m_cascadeEnd[i],1.0f},
				{-xn,-yn,m_cascadeEnd[i],1.0f},
				//far Face
				{xf,yf,m_cascadeEnd[i + 1],1.0f},
				{-xf,yf,m_cascadeEnd[i + 1],1.0f},
				{xf,-yf,m_cascadeEnd[i + 1],1.0f},
				{-xf,-yf,m_cascadeEnd[i + 1],1.0f}
			};
	
			glm::vec4 centerPos = glm::vec4(0.0f);
			for (uint32_t j = 0; j < 8; ++j)
			{
				frustumCorners[j] = camInv * frustumCorners[j];
				centerPos += frustumCorners[j];
			}
			centerPos /= 8.0f;

		
			float radius = 0.0f;
			for (uint32_t j = 0; j < 8; ++j)
			{
				float distance = glm::length(frustumCorners[j] - centerPos);
				radius = std::max(radius, distance);
			}
	
			// why we need this code?
			radius = std::ceil(radius * 16.0f) / 16.0f;

			// using radius ,  we made aabb box
			glm::vec3 maxExtents = glm::vec3(radius, radius, radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 shadowCamPos = glm::vec3(centerPos) + (glm::normalize(lightDirection) * (minExtents.z));
			glm::mat4 lightViewMatrix = glm::lookAt(shadowCamPos, glm::vec3(centerPos), glm::vec3(0.0f, 1.0f, 0.0f));

			glm::vec3 cascadeExtents = maxExtents - minExtents;

			glm::mat4 lightOrthoMatrix;
			if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
				lightOrthoMatrix = glm::orthoLH_ZO(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z) ;
			else
				lightOrthoMatrix = glm::orthoLH_NO(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y,0.0f, cascadeExtents.z) ;

			glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			shadowOrigin = lightOrthoMatrix * lightViewMatrix * shadowOrigin;
			shadowOrigin = shadowOrigin * (resoultion / 2.0f);

			glm::vec4 roundedOrigin = glm::round(shadowOrigin);
			glm::vec4 roundOffset = roundedOrigin - shadowOrigin;

			roundOffset = roundOffset * (2.0f / resoultion);
			roundOffset.z = 0.0f;
			roundOffset.w = 0.0f;

			lightOrthoMatrix[3] += roundOffset;
			m_shadowOrthoProj[i] = lightOrthoMatrix * lightViewMatrix;
		}
	}
	void ShadowMappingPass::Execute(Ref<Scene>& scene)
	{

		std::vector<glm::mat4> shadowTransforms;

		entt::registry& registry = scene->GetRegistry();
		auto lightView = registry.view<TransformComponent, LightComponent,TagComponent>();
		int dirLightCount = 0;
		int pointLightCount = 0;
		int spotLightCount = 0;

		for (auto& entity : lightView)
		{
			TransformComponent& transcomp = lightView.get<TransformComponent>(entity);
			TagComponent& tag = lightView.get<TagComponent>(entity);

			glm::vec3 lightPos = transcomp.Translation;
			glm::mat4 transform = transcomp.GetTransform();
			LightComponent& comp = lightView.get<LightComponent>(entity);
			glm::vec3 rightVector = glm::vec3(transform[0][0], transform[1][0], transform[2][0]);
			glm::vec3 upVector = glm::vec3(transform[0][1], transform[1][1], transform[2][1]);
			glm::vec3 forward = glm::vec3(transform[0][2], transform[1][2], transform[2][2]);
			comp.lightDirection = forward;
			transformConstantBuffer->Bind(0, Type::Vetex);
			uint32_t resoultion = 0;
			if (comp.type == LightComponent::LightType::Directional && dirLightCount ==0)
			{
				resoultion = 2048;
				
				csmMatrices->Bind(1, Type::Geometry);
				CsmPass(forward, upVector, resoultion);
				CsmMatrix matrix;
				matrix.matrices[0] = m_shadowOrthoProj[0];
				matrix.matrices[1] = m_shadowOrthoProj[1];
				matrix.matrices[2] = m_shadowOrthoProj[2];
				matrix.matrices[3] = m_shadowOrthoProj[3];
				matrix.matrices[4] = m_shadowOrthoProj[4];

				DirlightTransform->dirlightmatrices[0] = m_shadowOrthoProj[0];
				DirlightTransform->dirlightmatrices[1] = m_shadowOrthoProj[1];
				DirlightTransform->dirlightmatrices[2] = m_shadowOrthoProj[2];
				DirlightTransform->dirlightmatrices[3] = m_shadowOrthoProj[3];
				DirlightTransform->dirlightmatrices[4] = m_shadowOrthoProj[4];

				for (int i = 0; i < 6; ++i)
				{
					glm::vec4 vclip = (*projectionMatrix) * glm::vec4(0.0f, 0.0f, m_cascadeEnd[i], 1.0f);
					DirlightTransform->cascadedEndClip[i].x = vclip.z;
				}
				csmMatrices->SetData(&matrix, sizeof(CsmMatrix), 0u);

				m_DirectionalLightShadow->Bind();
				RenderCommand::SetViewport(0, 0, resoultion, resoultion);
				m_DirectionalLightShadow->DetachAll();
				//Ref<DepthStencilView> view = DepthStencilView::Create(TextureType::Texture2DArray, comp.shadowMap, TextureFormat::DEPTH32, 0, 0,5);
				Ref<DepthStencilView> view = DepthStencilView::Create(TextureType::Texture2DArray,m_DirLightShadowMap, TextureFormat::DEPTH32, 0, 0, 5);

				m_DirectionalLightShadow->AttachDepthTexture(view, AttachmentType::Depth);
				m_DirectionalLightShadow->Clear();
				m_DirectionalshadowMappingShader->Bind();
				DrawModel(registry);
				m_DirectionalshadowMappingShader->UnBind();
				m_DirectionalLightShadow->UnBind();

				m_ColorBuffer->Bind();
				m_ColorBuffer->DetachAll();
				m_ColorBuffer->AttachTexture(comp.debugMap, AttachmentType::Color_0, TextureType::Texture2D, 0);
				m_ColorBuffer->Clear();

				Ref<TextureShaderView> shaderview = TextureShaderView::Create(TextureType::Texture2D, m_DirLightShadowMap, TextureFormat::DEPTH32, 0, 1, comp.textureindex, 1);

				m_DebugShadowShader->Bind();
				shaderview->Bind(0, ShaderType::PS);
				m_SamplerForDebug->Bind(0);

				RenderCommand::DrawIndexed(m_quad);
				m_SamplerForDebug->UnBind(0);
				m_DebugShadowShader->UnBind();
				m_ColorBuffer->UnBind();

				dirLightCount++;
			}
			else if (comp.type == LightComponent::LightType::Spot && spotLightCount <8)
			{
				resoultion=512;
				SpotMatrix Spotlightdata;
				float angle = (comp.outerCutOff) * 2.0f;
				if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
					shadowProj = glm::perspectiveLH_ZO(glm::radians(angle), 1.0f, comp.near_plane, comp.far_plane);
				else
					shadowProj = glm::perspectiveLH_NO(glm::radians(angle), 1.0f, comp.near_plane, comp.far_plane);

				Spotlightdata.matrix = shadowProj * glm::lookAt(lightPos, lightPos + forward, upVector);
				DirlightTransform->spotlightmatrices[spotLightCount] = Spotlightdata.matrix;
			
				spotlightMatrix->Bind(1, Type::Vetex);
				spotlightMatrix->SetData(&Spotlightdata, sizeof(SpotMatrix), 0);

				m_SpotLightShadow->Bind();
				RenderCommand::SetViewport(0, 0, resoultion, resoultion);
				m_SpotLightShadow->DetachAll();
				// TODO
				Ref<DepthStencilView> depthview = DepthStencilView::Create(TextureType::Texture2D, m_SpotLightShadowMap, TextureFormat::DEPTH32, 0, spotLightCount, 1);
				m_SpotLightShadow->AttachDepthTexture(depthview, AttachmentType::Depth);
				//m_DirectionalLightShadow->AttachTexture(, AttachmentType::Depth, TextureType::Texture2D, 0);
				m_SpotLightShadow->Clear();

				m_SpotLightShadowMappingShader->Bind();
				DrawModel(registry);
				m_SpotLightShadowMappingShader->UnBind();

				m_SpotLightShadow->UnBind();

				Ref<TextureShaderView> view = TextureShaderView::Create(TextureType::Texture2D, m_SpotLightShadowMap, TextureFormat::DEPTH32, 0, 1, spotLightCount, 1);

				m_ColorBuffer->Bind();
				m_ColorBuffer->DetachAll();
				m_ColorBuffer->AttachTexture(comp.debugMap, AttachmentType::Color_0, TextureType::Texture2D, 0);
				m_ColorBuffer->Clear();

				m_DebugShadowShader->Bind();
				view->Bind(0, ShaderType::PS);
				
				m_SamplerForDebug->Bind(0);
				RenderCommand::DrawIndexed(m_quad);
				m_SamplerForDebug->UnBind(0);
				m_DebugShadowShader->UnBind();
				m_ColorBuffer->UnBind();
				spotLightCount++;
			}
			else if (comp.type == LightComponent::LightType::Point && pointLightCount< 4)
			{
				float bias = RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? -1.0f : 1.0f;
				if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
				{
					shadowProj = glm::perspectiveRH_NO(glm::radians(90.0f), 1.0f, comp.near_plane, comp.far_plane);
					shadowTransforms.push_back(shadowProj *
						glm::lookAtRH(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAtRH(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAtRH(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f * bias)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAtRH(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f * bias)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAtRH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAtRH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
				}
				else
				{
					shadowProj = glm::perspectiveLH_ZO(glm::radians(90.0f), 1.0f, comp.near_plane, comp.far_plane);
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f * bias)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f * bias)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f)));
				}
				resoultion = 512;
				comp.lightindex = pointLightCount;
				shadowMapMatrices->Bind(1, Type::Geometry);
				shadowMapMatrices->SetData(shadowTransforms.data(), sizeof(ShadowMatrix), 0);
				shadowTransforms.clear();
				m_PointLightShadow->Bind();
				RenderCommand::SetViewport(0, 0, resoultion, resoultion);
				m_PointLightShadow->DetachAll();
				//m_PointLightShadow->AttachTexture(m_PointLightShadowMap, AttachmentType::Depth, TextureType::TextureCubeArray, 0, pointLightCount,6);
				Ref<DepthStencilView> depthview = DepthStencilView::Create(TextureType::Texture2DArray, m_PointLightShadowMap, TextureFormat::DEPTH32,0, pointLightCount*6, 6);
				m_PointLightShadow->AttachDepthTexture(depthview,AttachmentType::Depth);
				m_PointLightShadow->Clear();

				m_PointshadowMappingShader->Bind();
				TextureLibrary::Load("white1x1tex")->Bind(0);
				DrawModel(registry);
				m_PointshadowMappingShader->UnBind();
				m_PointLightShadow->UnBind();

				Ref<TextureShaderView> view = TextureShaderView::Create(TextureType::Texture2D, m_PointLightShadowMap, TextureFormat::DEPTH32, 0, 1, (pointLightCount * 6)  + comp.textureindex,1);

				m_ColorBuffer->Bind();
				m_ColorBuffer->DetachAll();
				m_ColorBuffer->AttachTexture(comp.debugMap, AttachmentType::Color_0, TextureType::Texture2D, 0);
				m_ColorBuffer->Clear();

				m_DebugShadowShader->Bind();
				view->Bind(0, ShaderType::PS);
				m_SamplerForDebug->Bind(0);

				RenderCommand::DrawIndexed(m_quad);
				m_SamplerForDebug->UnBind(0);
				m_DebugShadowShader->UnBind();
				m_ColorBuffer->UnBind();
				pointLightCount++;
			}
		}	
	}
}

