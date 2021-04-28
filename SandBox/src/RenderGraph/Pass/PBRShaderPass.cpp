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

		
	
		/*materials[0].SetTexture("Asset/textures/PBR/rusted_iron/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/normal.png", imgSamp, Material::TextureType::Normal);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[1].SetTexture("Asset/textures/PBR/gold/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[1].SetTexture("Asset/textures/PBR/gold/normal.png", imgSamp, Material::TextureType::Normal);
		materials[1].SetTexture("Asset/textures/PBR/gold/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[1].SetTexture("Asset/textures/PBR/gold/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[1].SetTexture("Asset/textures/PBR/gold/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[2].SetTexture("Asset/textures/PBR/grass/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[2].SetTexture("Asset/textures/PBR/grass/normal.png", imgSamp, Material::TextureType::Normal);
		materials[2].SetTexture("Asset/textures/PBR/grass/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[2].SetTexture("Asset/textures/PBR/grass/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[2].SetTexture("Asset/textures/PBR/grass/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[3].SetTexture("Asset/textures/PBR/plastic/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[3].SetTexture("Asset/textures/PBR/plastic/normal.png", imgSamp, Material::TextureType::Normal);
		materials[3].SetTexture("Asset/textures/PBR/plastic/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[3].SetTexture("Asset/textures/PBR/plastic/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[3].SetTexture("Asset/textures/PBR/plastic/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[4].SetTexture("Asset/textures/PBR/wall/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[4].SetTexture("Asset/textures/PBR/wall/normal.png", imgSamp, Material::TextureType::Normal);
		materials[4].SetTexture("Asset/textures/PBR/wall/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[4].SetTexture("Asset/textures/PBR/wall/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[4].SetTexture("Asset/textures/PBR/wall/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);*/

		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));
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

		light[0].info.lightPosition = glm::vec3(10.0f, 10.0f, -20.0f);
		light[1].info.lightPosition = { -10.0f,10.0f,-20.0f };
		light[2].info.lightPosition = { 10.0f,-10.0f,-20.0f };
		light[3].info.lightPosition = { -10.0f,-10.0f,-20.0f };

		light[0].info.diffuse = { 300.0f,300.0f,300.0f };
		light[1].info.diffuse = { 300.0f,300.0f,300.0f };
		light[2].info.diffuse = { 300.0f,300.0f,300.0f };
		light[3].info.diffuse = { 300.0f,300.0f,300.0f };
	}
	void PBRShaderPass::Execute(Ref<Scene>& scene)
	{
		RenderCommand::SetDefaultFrameBuffer();
		RenderCommand::SetViewport(0, 0, 1600, 900);
		RenderCommand::SetCullMode(CullMode::Back);
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		Entity mainCamera = scene->GetPrimaryCameraEntity();
		glm::vec3 tc = mainCamera.GetComponent<TransformComponent>().Translation;
		const glm::mat4& proj = mainCamera.GetComponent<CameraComponent>().Camera.GetProjection();

		m_PBRShader->Bind();
		m_PBRShader->SetMat4("u_ViewProjection", proj *(*viewMatrix), ShaderType::VS);
		m_PBRShader->SetFloat3("u_viewPosition", tc, ShaderType::VS);
		for (int i = 0; i < 4; ++i)
		{
			std::string lightname = "pointLight[" + std::to_string(i) + "].";
			m_PBRShader->SetFloat3(lightname + "position", light[i].info.lightPosition, ShaderType::PS);
			m_PBRShader->SetFloat3(lightname + "diffuse", light[i].info.diffuse, ShaderType::PS);
		}
		m_PBRShader->SetFloat3("material.albedo", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
		m_PBRShader->SetFloat("material.metallic", 1.0f, ShaderType::PS);
		m_PBRShader->SetFloat("material.roughness", 1.0f, ShaderType::PS);
		m_PBRShader->SetFloat("material.ambientocclusion", 1.0f, ShaderType::PS);

		entt::registry& registry = scene->GetRegistry();
		auto group = registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
		int index = 0;
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
			meshComponent.Bind();
			RenderCommand::DrawIndexed(meshComponent.vertexArray);
		}

		m_FlatColorShader->Bind();
		m_FlatColorShader->SetMat4("u_ViewProjection", proj * (*viewMatrix), ShaderType::VS);
		m_FlatColorShader->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ShaderType::PS);
		for (int i = 0; i < 4; ++i)
		{
			sphere->SetTranslation(light[i].info.lightPosition);
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
	}
}