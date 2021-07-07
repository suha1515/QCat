#include "ShadowMappingPass.h"


namespace QCat
{
	ShadowMappingPass::ShadowMappingPass(unsigned int level, const std::string& name)
	{
		this->level = level;
		this->name = name;

		AttachmentSpecification pointdepthbuffer = { {FramebufferUsage::Depth,TextureType::TextureCube,TextureFormat::DEPTH32,"DepthBuffer"} };
		pointdepthbuffer.Height = 1024;
		pointdepthbuffer.Width = 1024;

		AttachmentSpecification directiondepthbuffer = {{FramebufferUsage::Depth,TextureType::Texture2D,TextureFormat::DEPTH32,"DepthBuffer"} };
		directiondepthbuffer.Height = 1024;
		directiondepthbuffer.Width = 1024;

		AttachmentSpecification colorBuffer = { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer"} };
		colorBuffer.Height = 1024;
		colorBuffer.Width = 1024;

		m_PointLightShadow = FrameBufferEx::Create(pointdepthbuffer);
		m_DirectionalLightShadow = FrameBufferEx::Create(directiondepthbuffer);
		m_ColorBuffer = FrameBufferEx::Create(colorBuffer);

		float bias = RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? -1.0f : 1.0f;
		shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
		

		shadowMapMatrices = ConstantBuffer::Create(sizeof(ShadowMatrix), 0);
		transformConstantBuffer = ConstantBuffer::Create(sizeof(Transform), 0);
		directionallightMatrix = ConstantBuffer::Create(sizeof(DirectionalLightViewProj), 0);
		
		Sampler_Desc desc;
		m_SamplerForDebug = SamplerState::Create(desc);
	}
	void ShadowMappingPass::Initialize()
	{
		m_PointshadowMappingShader = ShaderLibrary::Load("Asset/shaders/glsl/ShadowMap/PointLightShadowMap.glsl");
		m_DirectionalshadowMappingShader = ShaderLibrary::Load("Asset/shaders/glsl/ShadowMap/DirectionalLightShadowMap.glsl");
		m_DebugShadowShader = ShaderLibrary::Load("Asset/shaders/glsl/ShadowMap/DebugShadow.glsl");

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

			glm::mat4 transform = transformcomp.GetTransform();
			Transform buffer;
			buffer.transform = transform;
			buffer.invtrnasform = glm::inverse(transform);
			transformConstantBuffer->SetData(&buffer, sizeof(Transform), 0);

			for (auto& mesh : meshcomp.GetMeshes())
			{
				RenderCommand::DrawIndexed(mesh);
			}
		}
	}
	void ShadowMappingPass::Execute(Ref<Scene>& scene)
	{
		std::vector<glm::mat4> shadowTransforms;

		entt::registry& registry = scene->GetRegistry();
		auto lightView = registry.view<TransformComponent, LightComponent>();
		int index = 0;
		for (auto& entity : lightView)
		{
			if (index < 1)
			{
				TransformComponent& transcomp = lightView.get<TransformComponent>(entity);
				glm::vec3 lightPos = transcomp.Translation;
				glm::mat4 transform = transcomp.GetTransform();
				LightComponent& comp = lightView.get<LightComponent>(entity);
				if (comp.type == LightComponent::LightType::Directional)
				{
					glm::vec3 rightVector = glm::vec3(transform[0][0], transform[1][0], transform[2][0]);
					glm::vec3 upVector = glm::vec3(transform[0][1], transform[1][1], transform[2][1]);
					glm::vec3 forward = glm::vec3(transform[0][2], transform[1][2], transform[2][2]);
					DirectionalLightViewProj data;
					data.viewProjMatrix = shadowProj * glm::lookAt(lightPos, lightPos + forward, upVector);
		
					directionallightMatrix->SetData(&data, sizeof(DirectionalLightViewProj), 0);

					transformConstantBuffer->Bind(0, Type::Vetex);
					directionallightMatrix->Bind(1, Type::Vetex);
					m_DirectionalLightShadow->Bind();
					RenderCommand::SetViewport(0, 0, 1024, 1024);
					m_DirectionalLightShadow->DetachAll();
					m_DirectionalLightShadow->AttachTexture(comp.shadowMap, AttachmentType::Depth, TextureType::Texture2D,0);
					m_DirectionalLightShadow->Clear();

					m_DirectionalshadowMappingShader->Bind();
					DrawModel(registry);
					m_DirectionalshadowMappingShader->UnBind();

					m_DirectionalLightShadow->UnBind();

					m_ColorBuffer->Bind();
					m_ColorBuffer->DetachAll();
					m_ColorBuffer->AttachTexture(comp.debugMap, AttachmentType::Color_0, TextureType::Texture2D, 0);
					m_ColorBuffer->Clear();
					
					m_DebugShadowShader->Bind();
					
					m_SamplerForDebug->Bind(0);
					comp.shadowMap->Bind(0);
					RenderCommand::DrawIndexed(m_quad);
					m_SamplerForDebug->UnBind(0);
					m_DebugShadowShader->UnBind();
					m_ColorBuffer->UnBind();

				}
				else if (comp.type == LightComponent::LightType::Point)
				{
					float bias = RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? -1.0f : 1.0f;
					bias = 1.0f;
					if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
					{
						shadowProj = glm::perspectiveRH(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
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
					shadowMapMatrices->SetData(shadowTransforms.data(), sizeof(ShadowMatrix), 0);
					
					Sampler_Desc desc;
					Ref<Texture> sliceTexture = Texture2D::Create(TextureFormat::DEPTH32, desc, 1024, 1024);
					transformConstantBuffer->Bind(0, Type::Vetex);
					shadowMapMatrices->Bind(1, Type::Geometry);

					m_PointLightShadow->Bind();
					RenderCommand::SetViewport(0, 0, 1024, 1024);
					m_PointLightShadow->DetachAll();
					m_PointLightShadow->AttachTexture(comp.shadowMap, AttachmentType::Depth, TextureType::TextureCube,0);
					m_PointLightShadow->Clear();

					m_PointshadowMappingShader->Bind();
					DrawModel(registry);		
					m_PointshadowMappingShader->UnBind();
					m_PointLightShadow->UnBind();


					QCAT_BOX box;
					box.width = 1024;
					box.height = 1024;
					TextureUtility::CopyCubeMapFace2D(comp.shadowMap, sliceTexture,((uint32_t)TextureType::TextureCube_PositiveX + comp.textureindex), 0, box);

					m_ColorBuffer->Bind();
					m_ColorBuffer->DetachAll();
					m_ColorBuffer->AttachTexture(comp.debugMap, AttachmentType::Color_0, TextureType::Texture2D, 0);
					m_ColorBuffer->Clear();

					m_DebugShadowShader->Bind();
					sliceTexture->Bind(0);
					m_SamplerForDebug->Bind(0);
					
					RenderCommand::DrawIndexed(m_quad);
					m_SamplerForDebug->UnBind(0);
					m_DebugShadowShader->UnBind();
					m_ColorBuffer->UnBind();

					//delete[] data;
				}	
			}
			index++;
		}	
	}	
}

