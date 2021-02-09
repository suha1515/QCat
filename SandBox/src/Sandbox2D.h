#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "QCat.h"
#include "Geometry/Cube.h"
#include "Geometry/Sphere.h"
#include "Geometry/Face.h"
#include "Light/Light.h"
#include "QCat/Renderer/DepthStencil.h"

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

		Ref<Face> face;

		Ref<Face> grass1;
		Ref<Face> grass2;
		Ref<Face> grass3;

		Ref<Face> window1;
		Ref<Face> window2;
		Ref<Face> window3;

		Ref<Cube> cube;
		Ref<Cube> cube2;

		Ref<Sphere> sphere;
	
		Ref<Light> Pointlight;

		bool blinn = false;
		bool gamma = false;

		Ref<Scene> m_ActiveScene;

		Ref<Shader> m_LightShader;
		Ref<Shader> m_FlatShader;

		glm::vec3 m_CameraPosition = { 0.5f,0.0f,-7.2f };
		glm::vec3 m_CameraRotation = { 0.0f,0.0f, 0.0f };

		float constant = 1.0f;
		float Linear = 0.09f;
		float quadratic = 0.032f;

		float cameraSpeed;
		float lastX, lastY;
		float yaw=90.f, pitch=-30.0f;
		glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, -0.5f, 1.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f,0.0f,0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f,0.0f,0.0f);
		bool firstMouse = true;

		glm::mat4 viewMatrix;
	};

}
