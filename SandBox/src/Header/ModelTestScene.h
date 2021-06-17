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
		Ref<Scene> m_ActiveScene;

		Ref<Shader> BlinnPhongShader;
		Entity  HumanModel;


		glm::vec3 camPos = { 0.0f,0.0f,-3.0f };
		glm::vec3 lightPos = { 0.0f,0.0f,0.0f };
		glm::vec3 modelPos = { 0.0f,0.0f,3.0f };
		glm::vec3 modelRot = { 0.0f,0.0f,0.0f };

		float value = 0.0f;
		Material mat;

		struct Camera
		{
			glm::mat4 projection;
			glm::mat4 view;
			glm::vec3 viewPos;
		};
		struct Transform
		{
			glm::mat4 transform;
			glm::mat4 invTransform;
			glm::mat4 boneMatrices[100];
		};
		struct Mat
		{
			glm::vec3 albedo;
			float shininess;
			float metallic;
			float roughness;
			float ambientocclusion;

			int IsAlbedoMap;
			int IsNormalMap;
			int IsMetallicMap;
			int IsRoughnessMap;
			int IsAoMap;
		};

		struct light
		{
			glm::vec3 position = { 0.0f,0.0f,0.0f };
			float padding;
			glm::vec3 diffuse = { 0.0f,0.0f,0.0f };
			float padding2;
			glm::vec3 ambient;
			float padding3;
			glm::vec3 specular;
			float constant;
			float linear;
			float quadratic;
		};
		struct Extra
		{
			int gamma;
			int flip;
		};
		Camera camData;
		Transform transformData;
		Mat materialData;
		light lightData[1];
		Extra extraData;

		Ref<ConstantBuffer> cameraBuffer;
		Ref<ConstantBuffer> transformBuffer;
		Ref<ConstantBuffer> materialBuffer;
		Ref<ConstantBuffer> lightBuffer;
		Ref<ConstantBuffer> extraBuffer;
	};

}