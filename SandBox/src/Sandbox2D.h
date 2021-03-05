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

		
	private:
		Ref<Shader> geometryShaderTest;
		Ref<VertexArray> vao;
	};

}