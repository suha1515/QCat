#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "QCat.h"
#include "Geometry/Cube.h"
#include "Geometry/Sphere.h"
#include "Light/Light.h"

namespace QCat
{
	class Sandbox2D : public Layer
	{
	public:
		Sandbox2D();
	    virtual ~Sandbox2D()=default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		bool OnWindowResize(WindowResizeEvent& e);

		void CameraUpdate(QCat::Timestep ts);
	private:
		//camera
		Entity m_Camera;

		Ref<Cube> cube;
		Ref<Cube> cube1;
		Ref<Cube> cube2;
		Ref<Cube> cube3;
		Ref<Cube> cube4;
		Ref<Cube> cube5;

		Ref<Sphere> sphere;
		Ref<Light> Dirlight;
		Ref<Light> Pointlight[4];
		Ref<Light> Spotlight;


		Ref<Scene> m_ActiveScene;

		Ref<Shader> m_LightShader;

		glm::vec3 m_CameraPosition = { 0.0f,0.0f,-2.0f };
		glm::vec3 m_CameraRotation = { 0.0f,0.0f, 0.0f };

		float cameraSpeed;
		float lastX, lastY;
		float yaw=90.f, pitch=0.0f;
		glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f,0.0f,0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f,0.0f,0.0f);
		bool firstMouse = true;

		glm::mat4 viewMatrix;
	};

}
