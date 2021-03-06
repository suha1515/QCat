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
	class ModelTestScene : public Layer
	{
	public:
		ModelTestScene();
		virtual ~ModelTestScene() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		bool OnWindowResize(WindowResizeEvent& e);


	private:
		Ref<Shader> BlinnPhongShader;
		Ref<Shader> visualizingNormalShader;
		Ref<Model>  HumanModel;

		glm::vec3 camPos = { 0.0f,0.0f,-3.0f };
		glm::vec3 modelRot = { 0.0f,0.0f,0.0f };

		float value = 0.0f;
	};

}