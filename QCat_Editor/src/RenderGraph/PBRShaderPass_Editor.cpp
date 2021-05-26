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

		RegisterInput(TextureInput::Make("ColorBuffer", m_ColorBuffer));
		RegisterInput(TextureInput::Make("DepthBuffer", m_DepthBuffer));
		RegisterInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
		RegisterInput(DataInput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));

		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		AttachmentSpecification spec = { { FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer1" },
										 { FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer" } };
		spec.Width = 1600;
		spec.Height = 900;
		m_Framebuffer = FrameBufferEx::Create(spec);


		cameraConstantBuffer = ConstantBuffer::Create(sizeof(CameraData), 0);
		transformConstantBuffer = ConstantBuffer::Create(sizeof(Transform), 1);
		materialConstantBuffer = ConstantBuffer::Create(sizeof(Mat), 2);
		lightConstantBuffer = ConstantBuffer::Create(sizeof(light)*4, 3);
		colorConstantBuffer = ConstantBuffer::Create(sizeof(color), 2);

		LayoutElement c(ShaderDataType::Struct,"struct");
		c.Add(ShaderDataType::Float, "a");
		c.Add(ShaderDataType::Struct, "b");
		c["b"].Add(ShaderDataType::Float, "a");
		c["b"].Add(ShaderDataType::Float, "b");

		c.Add(ShaderDataType::Array, "c");
		c["c"].ArraySet(ShaderDataType::Float, 2);
		c.Add(ShaderDataType::Array, "d");
		c["d"].ArraySet(ShaderDataType::Struct, 2);
		c["d"].GetArrayElement().Add(ShaderDataType::Float, "a");
		c["d"].GetArrayElement().Add(ShaderDataType::Float, "b");

		c.Add(ShaderDataType::Float2, "e");
		c.Add(ShaderDataType::Struct, "f");
		c["f"].Add(ShaderDataType::Float3, "a");
		c["f"].Add(ShaderDataType::Struct, "b");
		c["f"]["b"].Add(ShaderDataType::Float, "a");
		c["f"]["b"].Add(ShaderDataType::Float, "b");
		c.Add(ShaderDataType::Mat4, "g");
		c.Add(ShaderDataType::Array, "h");
		c["h"].ArraySet(ShaderDataType::Array, 5);
		c["h"].GetArrayElement().ArraySet(ShaderDataType::Float, 5);
		c.Add(ShaderDataType::Mat3, "i");

		////b["material"].members

		////b.Finalize();
		c.Finalize();

		ElementBuffer buffer(c);
		buffer["a"s] = 1.0f;
		buffer["c"s][0] = 2.0f;
		buffer["b"s]["a"s] = 3.0f;
		float b = buffer["a"s];
		b = buffer["b"s]["a"s];
		b = buffer["c"s][0];
		buffer["f"s]["a"s] = glm::vec3(1.0f, 2.0f, 3.0f);
		glm::vec3 bb = buffer["f"s]["a"s];
		buffer["d"s][1]["a"s] = 4.0f;
		b = buffer["d"s][1]["a"s];
		buffer["f"s]["b"s]["b"s] = 5.0f;
		b = buffer["f"s]["b"s]["b"s];

		buffer["h"][0][0] = 2.0f;
		buffer["h"][3][2] = 3.0f;
		b = buffer["h"][0][0];
		b = buffer["h"][3][2];

		glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 4.0f));
		buffer["g"] = mat;
		glm::mat4 mat2 = buffer["g"] ;
		buffer["i"] = glm::mat3(1.0f);
		glm::mat3 mat3 = buffer["i"];

		//Layout b("Struct");

		LayoutElement lay(ShaderDataType::Struct,"Struct");
		lay.Add(ShaderDataType::Mat4, "Transform");
		lay.Add(ShaderDataType::Mat4, "InvTransform");
		
		transformBuffer = CreateRef<ElementBuffer>(lay);
	}
	void PBRShaderPass::Initialize()
	{
		m_PBRShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/PBR_PointLight.hlsl" : "Asset/shaders/glsl/PBR/PBR_PointLight.glsl");
		m_FlatColorShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/FlatColor.hlsl" : "Asset/shaders/glsl/FlatColor.glsl");
		m_SkyBoxShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/MakeHDRCubeMap.hlsl" : "Asset/shaders/glsl/PBR/MakeHDRCubeMap.glsl");


	/*	m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("environmentMap", 0, ShaderType::PS);

		m_PBRShader->Bind();
		m_PBRShader->SetInt("material.albedoMap", 0, ShaderType::PS);
		m_PBRShader->SetInt("material.normalMap", 1, ShaderType::PS);
		m_PBRShader->SetInt("material.metallicMap", 2, ShaderType::PS);
		m_PBRShader->SetInt("material.roughnessMap", 3, ShaderType::PS);
		m_PBRShader->SetInt("material.aoMap", 4, ShaderType::PS);
		m_PBRShader->SetInt("irradianceMap", 5, ShaderType::PS);
		m_PBRShader->SetInt("prefilterMap", 6, ShaderType::PS);
		m_PBRShader->SetInt("brdfLUT", 7, ShaderType::PS);*/

	}
	void PBRShaderPass::Execute(Ref<Scene>& scene)
	{
		m_Framebuffer->Bind();
		Texture_Desc desc = m_ColorBuffer->GetDesc();
		RenderCommand::SetViewport(0, 0, desc.Width, desc.Height);
		RenderCommand::SetCullMode(CullMode::Back);
		m_Framebuffer->DetachAll();
		m_Framebuffer->AttachTexture(m_ColorBuffer, AttachmentType::Color_0, TextureType::Texture2D, 0);
		m_Framebuffer->AttachTexture(m_DepthBuffer, AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		m_Framebuffer->Clear();

		//Entity mainCamera = scene->GetPrimaryCameraEntity();
		//glm::vec3 tc = mainCamera.GetComponent<TransformComponent>().Translation;
		//\const glm::mat4& proj = mainCamera.GetComponent<CameraComponent>().Camera.GetProjection();
		
		glm::vec3 tc = glm::inverse(*viewMatrix)[3];
		const glm::mat4& proj = (*projectionMatrix);
		entt::registry& registry = scene->GetRegistry();

		camData.projection = proj;
		camData.position = tc;
		camData.view = *viewMatrix;

		cameraConstantBuffer->SetData(&camData, sizeof(CameraData), 0);
		m_PBRShader->Bind();
		//m_PBRShader->SetMat4("u_ViewProjection", proj *(*viewMatrix), ShaderType::VS);
		//m_PBRShader->SetFloat3("u_viewPosition", tc, ShaderType::VS);
		auto lightView = registry.view<TransformComponent, LightComponent>();
		int index = 0;
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			LightComponent& comp = lightView.get<LightComponent>(entity);

			std::string lightname = "pointLight[" + std::to_string(index) + "].";
			//m_PBRShader->SetFloat3(lightname + "position", lightPos, ShaderType::PS);
			//m_PBRShader->SetFloat3(lightname + "diffuse", comp.diffuse, ShaderType::PS);
			litData[index].position = lightPos;
			litData[index].diffuse = comp.diffuse;
			index++;
		}
		lightConstantBuffer->SetData(&litData, sizeof(light), 0);
		matData.albedo = glm::vec3(1.0f, 1.0f, 1.0f);
		matData.metallic = 1.0f;
		matData.roughness = 1.0f;
		matData.ambientocclusion = 1.0f;


		//m_PBRShader->SetFloat3("material.albedo", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
		//m_PBRShader->SetFloat("material.metallic", 1.0f, ShaderType::PS);
		//m_PBRShader->SetFloat("material.roughness", 1.0f, ShaderType::PS);
		//m_PBRShader->SetFloat("material.ambientocclusion", 1.0f, ShaderType::PS);

		cameraConstantBuffer->Bind(0, Type::Vetex);
		transformConstantBuffer->Bind(1, Type::Vetex);
		materialConstantBuffer->Bind(2, Type::Pixel);
		lightConstantBuffer->Bind(3, Type::Pixel);

		auto group = registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
		for (auto entity : group)
		{
			glm::mat4 transform = group.get<TransformComponent>(entity).GetTransform();
			Material& mat = group.get<MaterialComponent>(entity).GetMaterial();
			(*transformBuffer)["Transform"s] = transform;
			(*transformBuffer)["InvTransform"s] = glm::inverse(transform);
			//transformData.transform = transform;
			//transformData.invtrnasform = glm::inverse(transform);
			transformConstantBuffer->SetData(transformBuffer->GetData(), transformBuffer->GetSize(), 0);

			//m_PBRShader->SetMat4("u_Transform", transform, ShaderType::VS);
			//m_PBRShader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
			matData.IsAlbedoMap = mat.IsThereTexture(Material::TextureType::Diffuse);
			matData.IsNormalMap = mat.IsThereTexture(Material::TextureType::Normal);
			matData.IsMetallicMap = mat.IsThereTexture(Material::TextureType::Metallic);
			matData.IsRoughnessMap = mat.IsThereTexture(Material::TextureType::Roughness);
			matData.IsAoMap = mat.IsThereTexture(Material::TextureType::AmbientOcclusion);
			materialConstantBuffer->SetData(&matData, sizeof(Mat), 0);
			//m_PBRShader->SetBool("material.IsAlbedoMap", mat.IsThereTexture(Material::TextureType::Diffuse), ShaderType::PS);
			//m_PBRShader->SetBool("material.IsNormalMap", mat.IsThereTexture(Material::TextureType::Normal), ShaderType::PS);
			//m_PBRShader->SetBool("material.IsMetallicMap", mat.IsThereTexture(Material::TextureType::Metallic), ShaderType::PS);
			//m_PBRShader->SetBool("material.IsRoughnessMap", mat.IsThereTexture(Material::TextureType::Roughness), ShaderType::PS);
			//m_PBRShader->SetBool("material.IsAoMap", mat.IsThereTexture(Material::TextureType::AmbientOcclusion), ShaderType::PS);
			//m_PBRShader->UpdateBuffer();

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

		colorConstantBuffer->Bind(2, Type::Pixel);
		colData.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		colorConstantBuffer->SetData(&colData, sizeof(color), 0);

		m_FlatColorShader->Bind();
		//m_FlatColorShader->SetMat4("u_ViewProjection", proj * (*viewMatrix), ShaderType::VS);
		//m_FlatColorShader->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ShaderType::PS);
		for (auto entity : lightView)
		{
			glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			sphere->SetTranslation(lightPos);
			sphere->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 transform = sphere->GetTransform();
			//m_FlatColorShader->SetMat4("u_Transform", transform, ShaderType::VS);
			//m_FlatColorShader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);

			transformData.transform = transform;
			transformData.invtrnasform = glm::inverse(transform);
			transformConstantBuffer->SetData(&transformData, sizeof(Transform), 0);

			//m_FlatColorShader->UpdateBuffer();
			sphere->Draw();
		}
		m_FlatColorShader->UnBind();

		//Draw HDR skyBox
		RenderCommand::SetCullMode(CullMode::None);
		RenderCommand::SetDepthFunc(COMPARISON_FUNC::LESS_EQUAL);
		{
			m_SkyBoxShader->Bind();
			//m_SkyBoxShader->SetMat4("u_Projection", proj, ShaderType::VS);
			//m_SkyBoxShader->SetMat4("u_View", *viewMatrix, ShaderType::VS);
			//m_SkyBoxShader->UpdateBuffer();
			m_HdrCubeMap->Bind(0);

			cube->Draw();
			m_SkyBoxShader->UnBind();
		}

		m_PBRShader->UnBind();
		m_Framebuffer->UnBind();
	}
}