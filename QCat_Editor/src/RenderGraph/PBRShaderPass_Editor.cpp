#include "PBRShaderPass_Editor.h"
#include "glm/gtc/type_ptr.hpp"
#include <API/Opengl/OpenGLBuffer.h>

namespace QCat
{
	PBRShaderPass::PBRShaderPass(unsigned int level, const std::string& name)
	{
		this->level = level;
		this->name = name;

		RegisterInput(TextureInput::Make("HdrCubeMap", m_HdrCubeMap));
		RegisterInput(TextureInput::Make("IrradianceCubeMap", m_IrradianceCubeMap));
		RegisterInput(TextureInput::Make("BRDFLutTexture", m_BRDFLutTextrue));
		RegisterInput(TextureInput::Make("PrefilterMap", m_PrefilterMap));
		RegisterInput(TextureInput::Make("DirectionalLightShadowMap", m_DirectionalLightShadowMap));
		RegisterInput(TextureInput::Make("PointLightShadowMap", m_PointLightShadowMap));
		RegisterInput(TextureInput::Make("SpotLightShadowMap", m_SpotLightShadowMap));


		RegisterInput(TextureInput::Make("ColorBuffer", m_ColorBuffer));
		RegisterInput(TextureInput::Make("DepthBuffer", m_DepthBuffer));
		RegisterInput(TextureInput::Make("IDBuffer", m_IDBuffer));

		RegisterInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
		RegisterInput(DataInput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));
		RegisterInput(DataInput<bool>::Make("DebugMode", m_DebugShadow, DataType::Bool));
		RegisterInput(DataInput<bool>::Make("SoftShadow", m_SoftShadow, DataType::Bool));


		RegisterInput(DataInput<ShadowMappingPass::LightMatrix>::Make("DirlightTransform", dirlightTransform, DataType::Struct));

		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		AttachmentSpecification spec = { { FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer1" },
										 { FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RED32_INTEGER,"IDBuffer" },
										 { FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer" } };
		spec.Width = 1600;
		spec.Height = 900;
		m_Framebuffer = FrameBufferEx::Create(spec);


		cameraConstantBuffer = ConstantBuffer::Create(sizeof(CameraData), 0);
		transformConstantBuffer = ConstantBuffer::Create(sizeof(Transform), 1);
		materialConstantBuffer = ConstantBuffer::Create(sizeof(Mat), 2);
		lightConstantBuffer = ConstantBuffer::Create(sizeof(Light), 3);
		colorConstantBuffer = ConstantBuffer::Create(sizeof(color), 2);
		dirlighbuffer = ConstantBuffer::Create(sizeof(ShadowMappingPass::LightMatrix), 4);
		cornerConstantBuffer = ConstantBuffer::Create(sizeof(Corners), 0);

		LayoutElement lay(ShaderDataType::Struct,"Struct");
		lay.Add(ShaderDataType::Mat4, "Transform");
		lay.Add(ShaderDataType::Mat4, "InvTransform");
		
		transformBuffer = CreateRef<ElementBuffer>(lay);

		Sampler_Desc desc;
		desc.MIN = Filtering::POINT;
		desc.MAG = Filtering::POINT;
		desc.MIP = Filtering::NONE;

		m_normalSampler = SamplerState::Create(desc);
	}
	void PBRShaderPass::Initialize()
	{
		m_PBRShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/PBR_PointLight.hlsl" : "Asset/shaders/glsl/PBR/PBR_PointLight.glsl");
		m_FlatColorShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/FlatColor.hlsl" : "Asset/shaders/glsl/FlatColor.glsl");
		m_SkyBoxShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/MakeHDRCubeMap.hlsl" : "Asset/shaders/glsl/PBR/MakeHDRCubeMap.glsl");
		m_BillboardShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/Billboard/Billboard.hlsl" : "Asset/shaders/glsl/Billboard/Billboard.glsl");
	}
	void PBRShaderPass::Execute(Ref<Scene>& scene)
	{
		Texture_Desc desc = m_ColorBuffer->GetDesc();
		RenderCommand::SetViewport(0, 0, desc.Width, desc.Height);
		RenderCommand::SetCullMode(CullMode::Back);

		m_Framebuffer->Bind();
		m_Framebuffer->DetachAll();
		int value = -1;
		m_Framebuffer->AttachTexture(m_ColorBuffer, AttachmentType::Color_0, TextureType::Texture2D, 0);
		m_Framebuffer->AttachTexture(m_IDBuffer, AttachmentType::Color_1, TextureType::Texture2D,0);
		m_Framebuffer->AttachTexture(m_DepthBuffer, AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		m_Framebuffer->Clear();
		std::dynamic_pointer_cast<Texture2D>(m_IDBuffer)->ClearData(0, 0, 0, m_IDBuffer->desc.Width, m_IDBuffer->desc.Height, TextureFormat::RED32_INTEGER, TextureDataType::INT, &value);

		//Entity mainCamera = scene->GetPrimaryCameraEntity();
		//glm::vec3 tc = mainCamera.GetComponent<TransformComponent>().Translation;
		//\const glm::mat4& proj = mainCamera.GetComponent<CameraComponent>().Camera.GetProjection();
		glm::mat4 invView = glm::inverse(*viewMatrix);
		glm::mat4 projView = (*projectionMatrix) * (*viewMatrix);
		glm::vec3 tc = invView[3];
		const glm::mat4& proj = (*projectionMatrix);
		entt::registry& registry = scene->GetRegistry();

		camData.projection = proj;
		camData.position = tc;
		camData.view = *viewMatrix;

		cameraConstantBuffer->SetData(&camData, sizeof(CameraData), 0);
		
		m_PBRShader->Bind();
		auto lightView = registry.view<TransformComponent, LightComponent, GuidComponent>();
		int dirLightCount = 0;
		int pointLightCount = 0;
		int spotLightCount = 0;
		Light light;
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			LightComponent& comp = lightView.get<LightComponent>(entity);
			if (comp.type == LightComponent::LightType::Directional && dirLightCount <= 0)
			{
				dirLightCount++;

				light.dirlight.diffuse = comp.diffuse;
				light.dirlight.isActive = true;
				light.dirlight.lightDirection = comp.lightDirection;
			}
			else if (comp.type == LightComponent::LightType::Point && pointLightCount < 4)
			{
				light.pointLight[pointLightCount].diffuse = comp.diffuse;
				light.pointLight[pointLightCount].isActive = true;
				light.pointLight[pointLightCount].position = lightPos;
				light.pointLight[pointLightCount].far_plane = comp.far_plane;
				light.pointLight[pointLightCount].near_plane = comp.near_plane;
				pointLightCount++;
			}
			else if (comp.type == LightComponent::LightType::Spot && spotLightCount < 8)
			{
				light.spotLight[spotLightCount].diffuse = comp.diffuse;
				light.spotLight[spotLightCount].isActive = true;
				light.spotLight[spotLightCount].position = lightPos;
				light.spotLight[spotLightCount].far_plane = comp.far_plane;
				light.spotLight[spotLightCount].cutoff = glm::cos(glm::radians(comp.cutoff));
				light.spotLight[spotLightCount].outercutoff = glm::cos(glm::radians(comp.outerCutOff));
				light.spotLight[spotLightCount].lightDirection = comp.lightDirection;
				spotLightCount++;
			}
		}
		ShadowMappingPass::LightMatrix data = *dirlightTransform;
		dirlighbuffer->SetData(&data, sizeof(ShadowMappingPass::LightMatrix), 0);

		light.isDebug = *m_DebugShadow;
		light.isSoft = *m_SoftShadow;
		lightConstantBuffer->SetData(&light, sizeof(Light), 0);

		matData.albedo = glm::vec3(1.0f, 1.0f, 1.0f);
		matData.metallic = 1.0f;
		matData.roughness = 1.0f;
		matData.ambientocclusion = 1.0f;

		cameraConstantBuffer->Bind(0, Type::Vetex);
		transformConstantBuffer->Bind(1, Type::Vetex);
		materialConstantBuffer->Bind(2, Type::Pixel);
		lightConstantBuffer->Bind(3, Type::Pixel);
		dirlighbuffer->Bind(4, Type::Pixel);

		auto group = registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent,GuidComponent>);
		for (auto entity : group)
		{
			MeshComponent& meshComponent = group.get<MeshComponent>(entity);

			if (meshComponent.vertexArray != nullptr)
			{
				glm::mat4 transform = group.get<TransformComponent>(entity).GetTransform();
				Material& mat = group.get<MaterialComponent>(entity).GetMaterial();
				//(*transformBuffer)["Transform"s] = transform;
				//(*transformBuffer)["InvTransform"s] = glm::inverse(transform);
				transformData.transform = transform;
				transformData.invtrnasform = glm::inverse(transform);
				transformData.id = group.get<GuidComponent>(entity).uid;
				transformConstantBuffer->SetData(&transformData, sizeof(Transform), 0);

				matData.IsAlbedoMap = mat.IsThereTexture(Material::TextureType::Diffuse) ? 1.0 : 0.0;
				matData.IsNormalMap = mat.IsThereTexture(Material::TextureType::Normal) ? 1.0 : 0.0;
				matData.IsMetallicMap = mat.IsThereTexture(Material::TextureType::Metallic) ? 1.0 : 0.0;
				matData.IsRoughnessMap = mat.IsThereTexture(Material::TextureType::Roughness) ? 1.0 : 0.0;
				matData.IsAoMap = mat.IsThereTexture(Material::TextureType::AmbientOcclusion) ? 1.0 : 0.0;

				matData.albedo = mat.diffuse;
				matData.metallic = mat.metallic;
				matData.roughness = mat.roughness;
				matData.ambientocclusion = mat.ao;

				materialConstantBuffer->SetData(&matData, sizeof(Mat), 0);

				mat.Bind(0, Material::TextureType::Diffuse);
				mat.Bind(1, Material::TextureType::Normal);
				mat.Bind(2, Material::TextureType::Metallic);
				mat.Bind(3, Material::TextureType::Roughness);
				mat.Bind(4, Material::TextureType::AmbientOcclusion);

				m_IrradianceCubeMap->Bind(5);
				m_PrefilterMap->Bind(6);
				m_BRDFLutTextrue->Bind(7);

				if (*m_SoftShadow)
				{
					if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
					{
						m_DirectionalLightShadowMap->Bind(8);
						m_PointLightShadowMap->Bind(9);
						m_SpotLightShadowMap->Bind(10);
						m_normalSampler->Bind(9);
					}
					else
					{
						m_DirectionalLightShadowMap->Bind(8);
						m_PointLightShadowMap->Bind(10);
						m_SpotLightShadowMap->Bind(12);
						m_normalSampler->Bind(9);
					}
				}
				else
				{
					if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
					{
						m_DirectionalLightShadowMap->Bind(8);
						m_PointLightShadowMap->Bind(9);
						m_SpotLightShadowMap->Bind(10);
						m_normalSampler->Bind(9);
					}
					else
					{
						m_DirectionalLightShadowMap->Bind(9);
						m_PointLightShadowMap->Bind(11);
						m_SpotLightShadowMap->Bind(13);
						m_normalSampler->Bind(9);
						m_normalSampler->Bind(11);
						m_normalSampler->Bind(13);
					}
				}
				RenderCommand::DrawIndexed(meshComponent.vertexArray);
			}
		}
		m_PBRShader->UnBind();

		colorConstantBuffer->Bind(2, Type::Pixel);
		colData.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		colorConstantBuffer->SetData(&colData, sizeof(color), 0);

		//Light
		/*m_FlatColorShader->Bind();
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			sphere->SetTranslation(lightPos);
			sphere->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 transform = sphere->GetTransform();

			transformData.transform = transform;
			transformData.invtrnasform = glm::inverse(transform);
			transformConstantBuffer->SetData(&transformData, sizeof(Transform), 0);
			sphere->Draw();
		}
		m_FlatColorShader->UnBind();*/

		m_BillboardShader->Bind();
		cornerConstantBuffer->Bind(1, Type::Vetex);
		Corners corner;
		glm::mat4 view = *viewMatrix;
		// For Vertices
		glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
		glm::vec3 cameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);
		float billboardsize = 0.5f;
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			corner.corners[0] = glm::vec4(lightPos + cameraRight * -0.5f * billboardsize + cameraUp * -0.5f * billboardsize, 1.0f) ;//bottom left
			corner.corners[1] = glm::vec4(lightPos + cameraRight * -0.5f * billboardsize + cameraUp * 0.5f * billboardsize,1.0f); //top left
			corner.corners[2] = glm::vec4(lightPos + cameraRight *  0.5f * billboardsize + cameraUp * -0.5f * billboardsize, 1.0f) ;//bottom right
			corner.corners[3] = glm::vec4(lightPos + cameraRight *  0.5f * billboardsize + cameraUp * 0.5f * billboardsize, 1.0f) ;//top right
			corner.id = lightView.get<GuidComponent>(entity).uid;
			cornerConstantBuffer->SetData(&corner, sizeof(Corners), 0);

			Ref<Texture2D> lightTex = TextureLibrary::Load("LightImage");
			if (lightTex)
				lightTex->Bind(0);

			RenderCommand::Draw(0, 4,RenderAPI::DrawMode::TRIANGLE_STRIP);
		}
		m_BillboardShader->UnBind();
		m_Framebuffer->DetachTexture(AttachmentType::Color_1);

		//Draw HDR skyBox
		RenderCommand::SetCullMode(CullMode::None);
		RenderCommand::SetDepthFunc(COMPARISON_FUNC::LESS_EQUAL);
		{
			m_SkyBoxShader->Bind();
			m_HdrCubeMap->Bind(0);

			cube->Draw();
			m_SkyBoxShader->UnBind();
		}

		m_Framebuffer->UnBind();
	}
}