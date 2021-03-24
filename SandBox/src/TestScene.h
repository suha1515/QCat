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
		
		Ref<Cube> cube;
		Ref<Sphere> sphere;

		Ref<Shader> BlinnPhongShader;
		Ref<Shader> screenShader;
		Ref<Shader> FlatShader;
		Ref<Shader> GaussianBlur;
		Ref<Shader> BloomShader;

		Ref<SamplerState> linearClamp;

		Ref<VertexArray> m_quad;
		Material floor;

		Ref<Texture2D> floorTexture;

		Ref<Scene> m_ActiveScene;

		Ref<FrameBuffer> offrendering;
		Ref<FrameBuffer> pingpongBuffer[2];
		bool first_iteration = true;
		bool horizontal = false;

		Light light[4];
		//light
		glm::vec3 LightPosition;
		float constant = 1.0f;
		float Linear = 0.09f;
		float quadratic = 0.032f;
		float height = 0.01f;
		float exposure = 1.0f;
		//Camera
		Entity m_Camera;
		float cameraSpeed;
		float lastX, lastY;
		float yaw = 90.f, pitch = -30.0f;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 viewMatrix;
	};

}