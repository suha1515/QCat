#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "QCat.h"
#include "Geometry/Cube.h"
#include "Geometry/Sphere.h"

namespace QCat
{
	class Sandbox2D : public Layer
	{
	public:
		Sandbox2D();
		virtual ~Sandbox2D() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		//camera
		Entity m_Camera;
		Entity m_Cube;

		Ref<Cube> cube;
		Ref<Sphere> sphere;

		Ref<Scene> m_ActiveScene;
		glm::vec3 m_CameraPosition = { 0.0f,0.0f,-2.0f };
		glm::vec3 m_CameraRotation = { 0.0f,0.0f, 0.0f };
	};

}
