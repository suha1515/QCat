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

		void CameraUpdate(QCat::Timestep ts);

		void RenderSkyObj(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos);
		void RenderLightObj(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos);
		void RenderNonLightObj(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos);
		void RenderScene(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos, const Ref<Shader>& shader);
	private:
		//camera
		Entity m_Camera;

		Ref<Face> face;

		Ref<Cube> cube;

		Ref<Sphere> sphere;

		Ref<CubeMap> cubeMap;
	
		Ref<Light> Pointlight;

		bool blinn = false;
		bool gamma = false;

		Ref<Scene> m_ActiveScene;

		Ref<Shader> m_LightShader;
		Ref<Shader> m_LightNormalShader;
		Ref<Shader> m_FlatShader;
		Ref<Shader> m_ScreenShader;
		Ref<Shader> m_ScreenDepthShader;
		Ref<Shader> m_SkyBoxShader;
		Ref<Shader> m_ReflectShader;
		Ref<Shader> m_CubeMapShader;
		Ref<Shader> m_ShadowMappingShader;
		Ref<Shader> m_BlinnPhongParallax;
		Ref<Texture2D> heightMap;

		Ref<FrameBuffer> framebuffer;
		Ref<FrameBuffer> screenframeBuffer;
		Ref<FrameBuffer> CubeFarameBuffer;
		Ref<FrameBuffer> DepthFrameBuffer;

		Ref<SamplerState> defaultSampler;
		Ref<SamplerState> shadowSampler;

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

		// position

		glm::vec3 grass1;
		glm::vec3 grass2;
		glm::vec3 grass3;

		glm::vec3 cube1;
		glm::vec3 cube2;

		glm::vec3 floor;
		glm::vec3 floorRot;
		glm::vec3 brickwall;

		glm::vec3 window1;
		glm::vec3 window2;
		glm::vec3 window3;

		glm::vec3 LightPosition;

		glm::vec3 backpackPos;
		glm::vec3 backpackRot;
		glm::vec3 ReflectObjPos;
		glm::vec3 cube3;

		Material woodFloor;
		Material grass;
		Material window;
		Material brick;
		Material Box;
		Material BasicMaterial;
		Material ReflectMaterial;
		Material CubeMapMat;

		glm::mat4 viewMatrix;

		Ref<VertexArray> m_quad;
		Ref<Texture2D> screenTexture;
		Ref<TextureCube> m_cubeTexture;
		Ref<TextureCube> m_cubeTexture2;
		Material temMat;

		//Model
		Ref<Model> diona;
		Ref<Model> muro;

		//Light
		Light dirLight;

		int depthBias=1000;
		float slopeBias = 6.0f;
		float depthClamp = 1.0f;
	};

}