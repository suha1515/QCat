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
		uint32_t width;
		uint32_t height;
		int num = 0;
		//Scene
		Ref<Scene> m_ActiveScene;

		Ref<Sphere> sphere;
		Ref<Cube> cube;
		
		Ref<Model> gun;
		Ref<Texture> HdrCubeMapTexture;
		Ref<Texture> IrradianceCubeMapTexture;
		Ref<Texture> PrefilterMap;
		Ref<Texture> BRDFLutTexture;
		glm::vec3 rotation = { 0.0f,3.2f,0.0f };

		Ref<Shader> PBRshader;
		Ref<Shader> Flatcolor;
		Ref<Shader> HdrToCube;
		Ref<Shader> HdrCubeMap;
		Ref<Shader> IrradianceMap;
		Ref<Shader> prefilter;
		Ref<Shader> BRDF2d;
		Ref<Texture2D> HDRImage;
		
		Ref<VertexArray> m_quad;

		Material RustedIron;
		Material Gold;
		Material grass;
		Material plastic;
		Material wall;
		Material pbrmat;
		Material materials[5];

		glm::mat4 captureProjection;
		glm::mat4 captureViews[6];
		//FrameBuffer
		Ref<FrameBufferEx> cubeMapPass;
		std::shared_ptr<FrameBuffer> cubeMapPass2;
		//light
		//Ref<Shader> HdrCubeMap2;

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
