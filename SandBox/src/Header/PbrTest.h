#pragma once

#include "QCat.h"
#include "../Geometry/Cube.h"
#include "../Geometry/Sphere.h"
#include "../Geometry/Face.h"
#include "../Light/Light.h"
#include "QCat/Renderer/DepthStencil.h"
#include "../Model/Model.h"
#include "../CubeMap/CubeMap.h"


namespace QCat
{
	class PbrTest : public Layer
	{
	public:
		PbrTest();
		virtual ~PbrTest() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		bool OnWindowResize(WindowResizeEvent& e);
		void CameraUpdate(QCat::Timestep ts);

	private:
		//Scene
		Ref<Scene> m_ActiveScene;

		Ref<Sphere> sphere;
		Ref<Cube> cube;
		
		Ref<Model> helmet;
		glm::vec3 rotation = { 0.0f,3.2f,0.0f };

		Ref<Shader> PBRshader;
		Ref<Shader> Flatcolor;
		
		Material pbrmat;

		//light
		Light light[4];
		glm::vec3 lightPosition;
		
		//Camera
		Entity m_Camera;
		float cameraSpeed;
		float lastX, lastY;
		float yaw = 90.f, pitch = 0.0f;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 viewMatrix;
	};

}
