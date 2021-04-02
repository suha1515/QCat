#pragma once

#include "QCat.h"
#include "Geometry/Cube.h"
#include "Geometry/Sphere.h"
#include "Geometry/Face.h"
#include "Light/Light.h"
#include "QCat/Renderer/DepthStencil.h"
#include "Model/Model.h"
#include "CubeMap/CubeMap.h"


namespace QCat
{
	class TestScene : public Layer
	{
	public:
		TestScene();
		virtual ~TestScene() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		bool OnWindowResize(WindowResizeEvent& e);
		void CameraUpdate(QCat::Timestep ts);

	private:
		Ref<Face> face;
		Ref<Cube> cube;
		Ref<Sphere> sphere;
		Ref<Model> backpack;
		glm::vec3 backpackPos = { -1.5f,-2.0f,1.5f };
		glm::vec3 backpackRot = { 1.6f,1.3f,0.0f };

		Ref<Shader> BlinnPhongShader;
		Ref<Shader> screenShader;
		Ref<Shader> FlatShader;
		Ref<Shader> GeometryPass;
		Ref<Shader> DeferredLighting;
		Ref<Shader> SSaoWrite;
		Ref<Shader> SSaoBlur;
		Ref<Shader> SSaoLight;
		Ref<Shader> SSaogeo;
		glm::vec2 screenSize=glm::vec2(0.0f,0.0f);
	
		Ref<SamplerState> linearClamp;

		Ref<VertexArray> m_quad;
		Material floor;
		Material brick;
		Material noTex;

		Ref<Texture2D> floorTexture;
		//Noise for SSAO
		Ref<Texture2D> noiseTexture;
		std::vector<glm::vec3> ssaokernel;
		std::vector<glm::vec4> ssaoNoise;

		Ref<Scene> m_ActiveScene;

		Ref<FrameBuffer> offrendering;
		Ref<FrameBuffer> ssao, ssaoBlur;

		float radius = 0.5f;
		float bias = 0.025f;

		Light light[1];
		//light
		glm::vec3 LightPosition;
		float constant = 0.1f;
		float Linear = 2.f;
		float quadratic = 4.f;
		float height = 0.01f;
		float exposure = 1.0f;

		//Camera
		Entity m_Camera;
		float cameraSpeed;
		float lastX, lastY;
		float yaw = 90.f, pitch = -30.0f;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraFront = glm::vec3(-0.6f, -0.3f, 0.7f);
		glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 viewMatrix;
	};

}