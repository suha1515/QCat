#pragma once
#include "QCat.h"
#include "../Geometry/Cube.h"
#include "../Geometry/Sphere.h"
#include "../RenderGraph/PBRRender/PBRRenderGraph.h"

namespace QCat
{
	class RenderGraphTest : public Layer
	{
	public:
		RenderGraphTest();
		virtual ~RenderGraphTest() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		void CameraUpdate(QCat::Timestep ts);

	private:
		//RenderGraph
		PBRRenderGraph renderGraph;
		Ref<Texture>hdrImage;

		Ref<Scene> m_ActiveScene;
		Entity ball[5];
		Entity model;
		Ref<Sphere> sphere;
		Ref<Cube> cube;
		//Camera
		Entity m_Camera;
		float cameraSpeed;
		float lastX, lastY;
		float yaw = 90.f, pitch = 0.0f;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


		Material materials[5];
		Material gunMat;
		Ref<glm::mat4> viewMatrix;
	};
}