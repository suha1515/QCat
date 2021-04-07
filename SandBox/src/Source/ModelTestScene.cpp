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

	ModelTestScene::ModelTestScene()
		:Layer("Sandbox2D")
	{
	}
	void ModelTestScene::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			visualizingNormalShader = Shader::Create("Asset/shaders/glsl/GeometryShader/VisualizingNormal.glsl");
			BlinnPhongShader = Shader::Create("Asset/shaders/glsl/Blinn-phong.glsl");
		}
		else
		{
			visualizingNormalShader = Shader::Create("visual","Asset/shaders/hlsl/GeometryShader/visualizeNormal_VS.hlsl", "Asset/shaders/hlsl/GeometryShader/visualizeNormal_PS.hlsl", "Asset/shaders/hlsl/GeometryShader/visualizeNormal_GS.hlsl");
			BlinnPhongShader = Shader::Create("blinn","Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl","Asset/shaders/hlsl/BlinnAndPhong_PS.hlsl");
		}
		BlinnPhongShader->SetInt("material.diffuse",0,ShaderType::PS);
		HumanModel = Model::Create("Asset/model/diona/Diona.fbx");
		HumanModel->SetTranslation({ 0.0f,-3.0f,0.0f });
		
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
		HumanModel->SetScale({ 0.5f,0.5f,0.5f });
		HumanModel->SetRotation(modelRot);

		BlinnPhongShader->Bind();
		BlinnPhongShader->SetMat4("u_ViewProjection", proj * view, ShaderType::VS);
		BlinnPhongShader->SetFloat3("viewPosition", camPos, ShaderType::VS);
		BlinnPhongShader->SetBool("blinn", true, ShaderType::PS);

		// Point Light 1
		BlinnPhongShader->SetFloat3("dirLight.direction", glm::vec3(0.5f,-0.5f,0.5f), ShaderType::PS);
		BlinnPhongShader->SetFloat3("dirLight.diffuse", glm::vec3(0.7f, 0.7f, 0.7f), ShaderType::PS);
		BlinnPhongShader->SetFloat3("dirLight.ambient", glm::vec3(0.3f, 0.3f, 0.3f), ShaderType::PS);
		BlinnPhongShader->SetFloat3("dirLight.specular", glm::vec3(1.0f,1.0f,1.0f), ShaderType::PS);

		HumanModel->Draw();


		BlinnPhongShader->UnBind();

		visualizingNormalShader->Bind();
		visualizingNormalShader->SetMat4("u_ViewProjection", proj * view, ShaderType::VS);
		HumanModel->Draw();
		visualizingNormalShader->UnBind();
	}

	void ModelTestScene::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

		ImGui::Begin("Settings");

		ImGui::DragFloat3("CamPos", glm::value_ptr(camPos), 0.1f);
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
