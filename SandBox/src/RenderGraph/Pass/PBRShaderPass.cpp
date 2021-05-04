#include "PBRShaderPass.h"
#include "glm/gtc/type_ptr.hpp"

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

		RegisterInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));

		RegisterInput(TextureInput::Make("ColorBuffer", m_ColorBuffer));
		RegisterInput(TextureInput::Make("DepthBuffer", m_DepthBuffer));

		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		AttachmentSpecification spec = { { FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer1" },
										 { FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer" } };
		m_Framebuffer = FrameBufferEx::Create(spec);

	}
	void PBRShaderPass::Initialize()
	{
		m_PBRShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/PBR_PointLight.hlsl" : "Asset/shaders/glsl/PBR/PBR_PointLight.glsl");
		m_FlatColorShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/FlatColor.hlsl" : "Asset/shaders/glsl/FlatColor.glsl");
		m_SkyBoxShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/MakeHDRCubeMap.hlsl" : "Asset/shaders/glsl/PBR/MakeHDRCubeMap.glsl");


		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("environmentMap", 0, ShaderType::PS);

		m_PBRShader->Bind();
		m_PBRShader->SetInt("material.albedoMap", 0, ShaderType::PS);
		m_PBRShader->SetInt("material.normalMap", 1, ShaderType::PS);
		m_PBRShader->SetInt("material.metallicMap", 2, ShaderType::PS);
		m_PBRShader->SetInt("material.roughnessMap", 3, ShaderType::PS);
		m_PBRShader->SetInt("material.aoMap", 4, ShaderType::PS);
		m_PBRShader->SetInt("irradianceMap", 5, ShaderType::PS);
		m_PBRShader->SetInt("prefilterMap", 6, ShaderType::PS);
		m_PBRShader->SetInt("brdfLUT", 7, ShaderType::PS);

	}
	void PBRShaderPass::Execute(Ref<Scene>& scene)
	{
		m_Framebuffer->Bind();
		Texture_Desc desc = m_ColorBuffer->GetDesc();
		RenderCommand::SetViewport(0, 0, desc.Width, desc.Height);
		RenderCommand::SetCullMode(CullMode::Back);
		m_Framebuffer->AttachTexture(m_ColorBuffer, AttachmentType::Color_0, TextureType::Texture2D, 0);
		m_Framebuffer->AttachTexture(m_DepthBuffer, AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		m_Framebuffer->Clear();

		Entity mainCamera = scene->GetPrimaryCameraEntity();
		glm::vec3 tc = mainCamera.GetComponent<TransformComponent>().Translation;
		const glm::mat4& proj = mainCamera.GetComponent<CameraComponent>().Camera.GetProjection();

		m_PBRShader->Bind();
		m_PBRShader->SetMat4("u_ViewProjection", proj *(*viewMatrix), ShaderType::VS);
		m_PBRShader->SetFloat3("u_viewPosition", tc, ShaderType::VS);
		entt::registry& registry = scene->GetRegistry();

		auto lightView = registry.view<TransformComponent,LightComponent>();
		int index = 0;
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			LightComponent& comp = lightView.get<LightComponent>(entity);

			std::string lightname = "pointLight[" + std::to_string(index) + "].";
			m_PBRShader->SetFloat3(lightname + "position", lightPos, ShaderType::PS);
			m_PBRShader->SetFloat3(lightname + "diffuse", comp.diffuse, ShaderType::PS);
			index++;
		}
		m_PBRShader->SetFloat3("material.albedo", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
		m_PBRShader->SetFloat("material.metallic", 1.0f, ShaderType::PS);
		m_PBRShader->SetFloat("material.roughness", 1.0f, ShaderType::PS);
		m_PBRShader->SetFloat("material.ambientocclusion", 1.0f, ShaderType::PS);

		
		auto group = registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
		for (auto entity : group)
		{
			glm::mat4 transform = group.get<TransformComponent>(entity).GetTransform();
			Material& mat = group.get<MaterialComponent>(entity).GetMaterial();
			m_PBRShader->SetMat4("u_Transform", transform, ShaderType::VS);
			m_PBRShader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
			m_PBRShader->SetBool("material.IsAlbedoMap", mat.IsThereTexture(Material::TextureType::Diffuse), ShaderType::PS);
			m_PBRShader->SetBool("material.IsNormalMap", mat.IsThereTexture(Material::TextureType::Normal), ShaderType::PS);
			m_PBRShader->SetBool("material.IsMetallicMap", mat.IsThereTexture(Material::TextureType::Metallic), ShaderType::PS);
			m_PBRShader->SetBool("material.IsRoughnessMap", mat.IsThereTexture(Material::TextureType::Roughness), ShaderType::PS);
			m_PBRShader->SetBool("material.IsAoMap", mat.IsThereTexture(Material::TextureType::AmbientOcclusion), ShaderType::PS);
			m_PBRShader->UpdateBuffer();

			mat.Bind(0, Material::TextureType::Diffuse);
			mat.Bind(1, Material::TextureType::Normal);
			mat.Bind(2, Material::TextureType::Metallic);
			mat.Bind(3, Material::TextureType::Roughness);
			mat.Bind(4, Material::TextureType::AmbientOcclusion);

			m_IrradianceCubeMap->Bind(5);
			m_PrefilterMap->Bind(6);
			m_BRDFLutTextrue->Bind(7);

			MeshComponent& meshComponent =  group.get<MeshComponent>(entity);
			for (auto& mesh : meshComponent.GetMeshes())
			{
				RenderCommand::DrawIndexed(mesh);
			}
			
		}

		m_FlatColorShader->Bind();
		m_FlatColorShader->SetMat4("u_ViewProjection", proj * (*viewMatrix), ShaderType::VS);
		m_FlatColorShader->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ShaderType::PS);
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			sphere->SetTranslation(lightPos);
			sphere->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 transform = sphere->GetTransform();
			m_FlatColorShader->SetMat4("u_Transform", transform, ShaderType::VS);
			m_FlatColorShader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
			m_FlatColorShader->UpdateBuffer();
			sphere->Draw();
		}

		m_FlatColorShader->UnBind();

		//Draw HDR skyBox
		RenderCommand::SetCullMode(CullMode::None);
		RenderCommand::SetDepthFunc(COMPARISON_FUNC::LESS_EQUAL);
		{
			m_SkyBoxShader->Bind();
			m_SkyBoxShader->SetMat4("u_Projection", proj, ShaderType::VS);
			m_SkyBoxShader->SetMat4("u_View", *viewMatrix, ShaderType::VS);
			m_SkyBoxShader->UpdateBuffer();
			m_HdrCubeMap->Bind(0);

			cube->Draw();
			m_SkyBoxShader->UnBind();
		}

		m_PBRShader->UnBind();
		m_Framebuffer->UnBind();
	}
}