#include "../Header/ModelTestScene.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"


#include <chrono>
#include <QCat/InputDevice/Mouse/Mouse.h>



namespace QCat
{
	namespace Utils
	{
		void UpdateTransform(Entity& entity, const glm::mat4 parentMatrix)
		{
			TransformComponent& transcomp = entity.GetComponent<TransformComponent>();
			transcomp.parentMatrix = parentMatrix;
			const glm::mat4 localMatrix = transcomp.GetTransform();
			auto child = entity.GetComponent<RelationShipComponent>().firstChild;

			while (child != Entity::emptyEntity)
			{
				UpdateTransform(child, localMatrix);
				child = child.GetComponent<RelationShipComponent>().nextSibling;
			}
		}
		void SetMaterial(Entity& entity, Material& mat)
		{
			if(entity.HasComponent<MaterialComponent>())
				entity.GetComponent<MaterialComponent>().material = mat;
			auto child = entity.GetComponent<RelationShipComponent>().firstChild;

			while (child != Entity::emptyEntity)
			{
				SetMaterial(child, mat);
				child = child.GetComponent<RelationShipComponent>().nextSibling;
			}
		}
	}
	
	ModelTestScene::ModelTestScene()
		:Layer("Sandbox2D")
	{
	}
	void ModelTestScene::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_ActiveScene = CreateRef<Scene>();

		HumanModel = ModelLoader::LoadModel("Asset/model/vampire/dancing_vampire.dae",m_ActiveScene);
		HumanModel.GetComponent<TransformComponent>().Scale = {0.1f,0.1f,0.1f};
		HumanModel.GetComponent<TransformComponent>().Rotation = { 0.0,0.0,0.0f };
		HumanModel.GetComponent<TransformComponent>().Translation = modelPos;

		Material mat;
		Sampler_Desc imgSamp;
		imgSamp.addressU = WrapingMode::REPEAT;
		imgSamp.addressV = WrapingMode::REPEAT;
		imgSamp.MIN = Filtering::LINEAR;
		imgSamp.MAG = Filtering::LINEAR;
		imgSamp.MIP = Filtering::LINEAR;
		mat.SetTexture("Asset/model/vampire/textures/Vampire_diffuse.png", imgSamp, Material::TextureType::Diffuse);
		//mat.SetTexture("Asset/model/vampire/textures/Vampire_normal.png", imgSamp, Material::TextureType::Normal);
		mat.SetTexture("Asset/model/vampire/textures/Vampire_specular.png", imgSamp, Material::TextureType::Specular);

		Utils::SetMaterial(HumanModel, mat);
		Utils::UpdateTransform(HumanModel, glm::mat4(1.0f));
		

		Entity pointLight = m_ActiveScene->CreateEntity("PointLight1");
		pointLight.GetComponent<TransformComponent>().Translation = glm::vec3(0.0f, 1.0f, 2.0f);
		auto& comp = pointLight.AddComponent<LightComponent>();
		comp.diffuse = { 1.0f,1.0f,1.0f };

		BlinnPhongShader = ShaderLibrary::Load("Asset/shaders/glsl/Blinn-phong_Animation.glsl");

		cameraBuffer = ConstantBuffer::Create(sizeof(Camera), 0);
		transformBuffer = ConstantBuffer::Create(sizeof(Transform), 1);
		materialBuffer = ConstantBuffer::Create(sizeof(Mat), 2);
		lightBuffer = ConstantBuffer::Create(sizeof(light), 3);
		extraBuffer = ConstantBuffer::Create(sizeof(Extra), 4);


		
		RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1.0f });
	}

	void ModelTestScene::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void ModelTestScene::OnUpdate(QCat::Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		RenderCommand::Clear();

		glm::mat4 proj = glm::perspective(glm::radians(45.0f), 16.f / 9.f, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.f, 1.0f, 0.0f));

		glm::vec3 tc = glm::inverse(view)[3];
		
		camData.projection = proj;
		camData.view = view;
		camData.viewPos = tc;
		cameraBuffer->SetData(&camData, sizeof(Camera), 0);

		auto& comp = HumanModel.GetComponent<TransformComponent>();
		comp.Translation = modelPos;
		comp.Rotation = modelRot;
		entt::registry& registry = m_ActiveScene->GetRegistry();
		auto lightView = registry.view<TransformComponent, LightComponent>();
		int index = 0;
		for (auto entity : lightView)
		{
			//glm::vec3 lightPos = lightView.get<TransformComponent>(entity).Translation;
			LightComponent& comp = lightView.get<LightComponent>(entity);
			lightView.get<TransformComponent>(entity).Translation = lightPos;
			lightData[index].position = lightPos;
			lightData[index].diffuse = comp.diffuse;
			lightData[index].ambient = comp.ambient;
			lightData[index].specular = comp.specular;
			lightData[index].constant = comp.constant;
			lightData[index].linear = comp.linear;
			lightData[index].quadratic = comp.quadratic;
			index++;
		}

		lightBuffer->SetData(&lightData, sizeof(light));
		materialData.albedo = glm::vec3(1.0f, 1.0f, 1.0f);
		materialData.shininess = 32.0f;
		materialData.metallic = 1.0f;
		materialData.roughness = 1.0f;
		materialData.ambientocclusion = 1.0f;

		//Animation Update
		auto& anim = HumanModel.GetComponent<AnimatorComponent>();
		anim.animator.UpdateAnimation(ts);
		auto& transforms = anim.animator.GetTransforms();
		for (int i = 0; i < transforms.size(); ++i)
		{
			transformData.boneMatrices[i] = transforms[i];
		}
		BlinnPhongShader->Bind();

		cameraBuffer->Bind(0, Type::Vetex);
		transformBuffer->Bind(1, Type::Vetex);
		materialBuffer->Bind(2, Type::Pixel);
		lightBuffer->Bind(3, Type::Pixel);

		auto group = registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
		for (auto entity : group)
		{
			glm::mat4 transform = group.get<TransformComponent>(entity).GetTransform();
			//glm::mat4 transform = glm::mat4(1.0f);

			Material& mat = group.get<MaterialComponent>(entity).GetMaterial();
			transformData.transform = transform;
			transformData.invTransform = glm::inverse(transform);
			transformBuffer->SetData(&transformData, sizeof(Transform), 0);

			materialData.IsAlbedoMap = mat.IsThereTexture(Material::TextureType::Diffuse);
			materialData.IsNormalMap = mat.IsThereTexture(Material::TextureType::Normal);
			materialData.IsMetallicMap = mat.IsThereTexture(Material::TextureType::Metallic);
			materialData.IsRoughnessMap = mat.IsThereTexture(Material::TextureType::Roughness);
			materialData.IsAoMap = mat.IsThereTexture(Material::TextureType::AmbientOcclusion);
			materialBuffer->SetData(&materialData, sizeof(Mat), 0);

			mat.Bind(0, Material::TextureType::Diffuse);
			mat.Bind(1, Material::TextureType::Normal);
			mat.Bind(2, Material::TextureType::Specular);
			mat.Bind(3, Material::TextureType::AmbientOcclusion);

			MeshComponent& meshComponent = group.get<MeshComponent>(entity);
			for (auto& mesh : meshComponent.GetMeshes())
			{
				RenderCommand::DrawIndexed(mesh);
			}

		}


		BlinnPhongShader->UnBind();
	}

	void ModelTestScene::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

		ImGui::Begin("Settings");

		ImGui::DragFloat3("CamPos", glm::value_ptr(camPos), 0.1f);
		ImGui::DragFloat3("LightPos", glm::value_ptr(lightPos), 0.1f);
		ImGui::DragFloat3("ModelPos", glm::value_ptr(modelPos), 0.1f);
		ImGui::DragFloat3("ModelRot", glm::value_ptr(modelRot), 0.1f);

		ImGui::End();


	}

	void ModelTestScene::OnEvent(QCat::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));


	}

	bool ModelTestScene::OnWindowResize(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();

		return false;
	}

}

